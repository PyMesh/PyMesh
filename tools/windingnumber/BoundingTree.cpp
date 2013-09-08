#include "BoundingTree.h"
#include "winding_number.h"
#include "tesselate_boundary.h"

#include <igl/remove_duplicate_vertices.h>

#include <iostream>
#include <limits>

template <typename Point>
WindingNumberMethod BoundingTree<Point>::method = EXACT_WINDING_NUMBER_METHOD;
template <typename Point>
double BoundingTree<Point>::min_max_w = 0;

static Eigen::MatrixXd dummyV;
template <typename Point>
BoundingTree<Point>::BoundingTree(
  const Eigen::MatrixXd & _V,
  const Eigen::MatrixXi & _F):
  parent(NULL),
  V(dummyV),
  SV(),
  F(),
  //boundary(igl::boundary_faces<Eigen::MatrixXi,Eigen::MatrixXi>(F))
  cap()
{
  // Remove any exactly duplicate vertices
  // Q: Can this ever increase the complexity of the boundary?
  // Q: Would we gain even more by remove almost exactly duplicate vertices?
  Eigen::MatrixXi SF,SVI,SVJ;
  igl::remove_duplicate_vertices(_V,_F,0.0,SV,SVI,SVJ,F);
  tesselate_boundary(F,cap);
  V = SV;
}

template <typename Point>
BoundingTree<Point>::BoundingTree(
  const BoundingTree & parent,
  const Eigen::MatrixXi & _F):
  parent(&parent),
  V(parent.V),
  SV(),
  F(_F),
  cap(tesselate_boundary(_F))
{
}

template <typename Point>
BoundingTree<Point>::~BoundingTree()
{
  using namespace std;
  // Delete children
  typename list<BoundingTree<Point>* >::iterator cit = children.begin();
  while(cit != children.end())
  {
    // clear the memory of this item
    delete (* cit);
    // erase from list, returns next element in iterator
    cit = children.erase(cit);
  }
}

template <typename Point>
const Eigen::MatrixXd & BoundingTree<Point>::getV() const
{
  return V;
}

template <typename Point>
const Eigen::MatrixXi & BoundingTree<Point>::getF() const
{
  return F;
}

template <typename Point>
const Eigen::MatrixXi & BoundingTree<Point>::getcap() const
{
  return cap;
}

template <typename Point>
void BoundingTree<Point>::grow()
{
  // Don't grow
  return;
}

template <typename Point>
bool BoundingTree<Point>::inside(const Point & /*p*/) const
{
  return true;
}

template <typename Point>
double BoundingTree<Point>::winding_number(const Point & p) const
{
  using namespace std;
  //cout<<"+"<<boundary.rows();
  // If inside then we need to be careful
  if(inside(p))
  {
    // If not a leaf then recurse
    if(children.size()>0)
    {
      // Recurse on each child and accumulate
      double sum = 0;
      for(
        typename list<BoundingTree<Point>* >::const_iterator cit = children.begin();
        cit != children.end();
        cit++)
      {
        switch(method)
        {
          case EXACT_WINDING_NUMBER_METHOD:
            sum += (*cit)->winding_number(p);
            break;
          case APPROX_SIMPLE_WINDING_NUMBER_METHOD:
            if((*cit)->max_simple_abs_winding_number(p) > min_max_w)
            {
              sum += (*cit)->winding_number(p);
            }
            break;
          default:
            assert(false);
            break;
        }
      }
      return sum;
    }else
    {
      return winding_number_all(p);
    }
  }else{
    // Otherwise we can just consider boundary
    // Q: If we using the "multipole" method should we also subdivide the
    // boundary case?
    if((cap.rows() - 2) < F.rows())
    {
      return winding_number_boundary(p);
    }else
    {
      // doesn't pay off to use boundary
      return winding_number_all(p);
    }
  }
}

template <typename Point>
double BoundingTree<Point>::winding_number_all(const Point & p) const
{
  double w = 0;
  ::winding_number_3(
    V.data(),
    V.rows(),
    F.data(),
    F.rows(),
    p.data(),
    1,
    &w);
  return w;
}

template <typename Point>
double BoundingTree<Point>::winding_number_boundary(const Point & p) const
{
  using namespace Eigen;
  using namespace std;

  double w = 0;
  ::winding_number_3(
    V.data(),
    V.rows(),
    cap.data(),
    cap.rows(),
    &p[0],
    1,
    &w);
  return w;
}

//template <typename Point>
//double BoundingTree<Point>::winding_number_approx_simple(
//  const Point & p, 
//  const double min_max_w)
//{
//  using namespace std;
//  if(max_simple_abs_winding_number(p) > min_max_w)
//  {
//    return winding_number(p);
//  }else
//  {
//    cout<<"Skipped! "<<max_simple_abs_winding_number(p)<<"<"<<min_max_w<<endl;
//    return 0;
//  }
//}

template <typename Point>
void BoundingTree<Point>::print(const char * tab)
{
  using namespace std;
  // Print all facets
  cout<<tab<<"["<<endl<<F<<endl<<"]";
  // Print children
  for(
      typename list<BoundingTree<Point>* >::iterator cit = children.begin();
      cit != children.end();
      cit++)
  {
    cout<<","<<endl;
    (*cit)->print((string(tab)+"").c_str());
  }
}

template <typename Point>
double BoundingTree<Point>::max_abs_winding_number(const Point & p) const
{
  return std::numeric_limits<double>::infinity();
}

template <typename Point>
double BoundingTree<Point>::max_simple_abs_winding_number(const Point & p) const
{
  using namespace std;
  return numeric_limits<double>::infinity();
}

// Explicit instanciation
template class BoundingTree<Eigen::Vector3d >;
