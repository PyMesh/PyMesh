#include "AABB.h"
#include "winding_number.h"

#include <igl/barycenter.h>
#include <igl/median.h>
#include <igl/doublearea.h>
#include <igl/per_face_normals.h>

#include <limits>
#include <vector>
#include <iostream>

// Minimum number of faces in a hierarchy element (this is probably dependent
// on speed of machine and compiler optimization)
#ifndef AABB_MIN_F
#  define AABB_MIN_F 100
#endif

template <typename Point>
void AABB<Point>::init()
{
  using namespace igl;
  using namespace Eigen;
  assert(max_corner.size() == 3);
  assert(min_corner.size() == 3);
  compute_min_max_corners();
  VectorXd dblA;
  doublearea(this->getV(),this->getF(),dblA);
  total_positive_area = dblA.sum()/2.0;
}

template <typename Point>
AABB<Point>::AABB(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F):
  BoundingTree<Point>(V,F),
  min_corner(),
  max_corner(),
  total_positive_area(std::numeric_limits<double>::infinity()),
  split_method(MEDIAN_ON_LONGEST_AXIS)
{
  init();
}

template <typename Point>
AABB<Point>::AABB(
  const BoundingTree<Point> & parent,
  const Eigen::MatrixXi & F):
  BoundingTree<Point>(parent,F),
  min_corner(),
  max_corner(),
  total_positive_area(std::numeric_limits<double>::infinity()),
  split_method(MEDIAN_ON_LONGEST_AXIS)
{
  init();
}

template <typename Point>
void AABB<Point>::grow()
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;
  //cout<<"cap.rows(): "<<this->getcap().rows()<<endl;
  //cout<<"F.rows(): "<<this->getF().rows()<<endl;

  // Base cases
  if(
    this->getF().rows() <= (AABB_MIN_F>0?AABB_MIN_F:0) ||
    (this->getcap().rows() - 2) >= this->getF().rows())
  {
    // Don't grow
    return;
  }

  // Compute longest direction
  int max_d = -1;
  double max_len = -numeric_limits<double>::infinity();
  for(int d = 0;d<min_corner.size();d++)
  {
    if( (max_corner[d] - min_corner[d]) > max_len )
    {
      max_len = (max_corner[d] - min_corner[d]);
      max_d = d;
    }
  }
  // Compute facet barycenters
  MatrixXd BC;
  barycenter(this->getV(),this->getF(),BC);


  // Blerg, why is selecting rows so difficult

  vector<int> id( this->getF().rows());
  double split_value;
  // Split in longest direction
  switch(split_method)
  {
    case MEDIAN_ON_LONGEST_AXIS:
      // Determine median
      median(BC.col(max_d),split_value);
      break;
    default:
      assert(false);
    case CENTER_ON_LONGEST_AXIS:
      split_value = 0.5*(max_corner[max_d] + min_corner[max_d]);
      break;
  }
  //cout<<"c: "<<0.5*(max_corner[max_d] + min_corner[max_d])<<" "<<
  //  "m: "<<split_value<<endl;;

  for(int i = 0;i<this->getF().rows();i++)
  {
    if(BC(i,max_d) <= split_value)
    {
      id[i] = 0; //left
    }else
    {
      id[i] = 1; //right
    }
  }

  const int lefts = (int) count(id.begin(),id.end(),0);
  const int rights = (int) count(id.begin(),id.end(),1);
  if(lefts == 0 || rights == 0)
  {
    // badly balanced base case (could try to recut)
    return;
  }
  assert(lefts+rights == this->getF().rows());
  MatrixXi leftF(lefts,  this->getF().cols());
  MatrixXi rightF(rights,this->getF().cols());
  int left_i = 0;
  int right_i = 0;
  for(int i = 0;i<this->getF().rows();i++)
  {
    if(id[i] == 0)
    {
      leftF.row(left_i++) = this->getF().row(i);
    }else if(id[i] == 1)
    {
      rightF.row(right_i++) = this->getF().row(i);
    }else
    {
      assert(false);
    }
  }
  assert(right_i == rightF.rows());
  assert(left_i == leftF.rows());
  // Finally actually grow children and Recursively grow
  AABB<Point> * leftAABB = new AABB<Point>(*this,leftF);
  leftAABB->grow();
  this->children.push_back(leftAABB);
  AABB<Point> * rightAABB = new AABB<Point>(*this,rightF);
  rightAABB->grow();
  this->children.push_back(rightAABB);
}

template <typename Point>
bool AABB<Point>::inside(const Point & p) const
{
  assert(p.size() == max_corner.size());
  assert(p.size() == min_corner.size());
  for(int i = 0;i<p.size();i++)
  {
    if( p(i) < min_corner(i) || p(i) >= max_corner(i))
    {
      return false;
    }
  }
  return true;
}

template <typename Point>
void AABB<Point>::compute_min_max_corners()
{
  using namespace std;
  // initialize corners
  for(int d = 0;d<min_corner.size();d++)
  {
    min_corner[d] =  numeric_limits<double>::infinity();
    max_corner[d] = -numeric_limits<double>::infinity();
  }

  // Loop over facets
  for(int i = 0;i<this->getF().rows();i++)
  {
    for(int j = 0;j<this->getF().cols();j++)
    {
      for(int d = 0;d<min_corner.size();d++)
      {
        min_corner[d] = 
          this->getV()(this->getF()(i,j),d) < min_corner[d] ?  
            this->getV()(this->getF()(i,j),d) : min_corner[d];
        max_corner[d] = 
          this->getV()(this->getF()(i,j),d) > max_corner[d] ?  
            this->getV()(this->getF()(i,j),d) : max_corner[d];
      }
    }
  }
}

template <typename Point>
double AABB<Point>::max_abs_winding_number(const Point & p) const
{
  using namespace std;
  // Only valid if not inside
  if(inside(p))
  {
    return numeric_limits<double>::infinity();
  }
  // Q: we know the total positive area so what's the most this could project
  // to? Remember it could be layered in the same direction.
  return numeric_limits<double>::infinity();
}

template <typename Point>
double AABB<Point>::max_simple_abs_winding_number(const Point & p) const
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;
  // Only valid if not inside
  if(inside(p))
  {
    return numeric_limits<double>::infinity();
  }
  // Max simple is the same as sum of positive winding number contributions of
  // bounding box

  // begin precomputation
  MatrixXd BV((int)pow(2.0,3.0),3);
  BV <<
    min_corner[0],min_corner[1],min_corner[2],
    min_corner[0],min_corner[1],max_corner[2],
    min_corner[0],max_corner[1],min_corner[2],
    min_corner[0],max_corner[1],max_corner[2],
    max_corner[0],min_corner[1],min_corner[2],
    max_corner[0],min_corner[1],max_corner[2],
    max_corner[0],max_corner[1],min_corner[2],
    max_corner[0],max_corner[1],max_corner[2];
  MatrixXi BF(2*2*3,3);
  BF <<
    0,6,4,
    0,2,6,
    0,3,2,
    0,1,3,
    2,7,6,
    2,3,7,
    4,6,7,
    4,7,5,
    0,4,5,
    0,5,1,
    1,5,7,
    1,7,3;
  MatrixXd BFN;
  per_face_normals(BV,BF,BFN);
  // end of precomputation

  // Only keep those with positive dot products
  MatrixXi PBF(BF.rows(),BF.cols());
  int pbfi = 0;
  Point p2c = 0.5*(min_corner+max_corner)-p;
  for(int i = 0;i<BFN.rows();i++)
  {
    if(p2c.dot(BFN.row(i)) > 0)
    {
      PBF.row(pbfi++) = BF.row(i);
    }
  }
  PBF.conservativeResize(pbfi,PBF.cols());
  double w = numeric_limits<double>::infinity();
  ::winding_number_3(
    BV.data(),
    BV.rows(),
    PBF.data(),
    PBF.rows(),
    p.data(),
    1,
    &w);
  return w;
}

// Explicit instanciation
template class AABB<Eigen::Vector3d >;
