
#include "SelfIntersectMesh.h"

#include <igl/REDRUM.h>
#include <igl/C_STR.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <exception>
#include <cassert>
#include <iostream>
#include <vector>

#ifdef MEX
#  include <mex.h>
#  undef assert
#  ifdef NDEBUG
#    define assert( isOK ) ((void)0) 
#  else
#    define assert( isOK ) ( (isOK) ? (void)0 : (void) mexErrMsgTxt(C_STR(__FILE__<<":"<<__LINE__<<": failed assertion `"<<#isOK<<"'"<<std::endl) ) )
#  endif
#endif

// References:
// http://minregret.googlecode.com/svn/trunk/skyline/src/extern/CGAL-3.3.1/examples/Polyhedron/polyhedron_self_intersection.cpp
// http://www.cgal.org/Manual/3.9/examples/Boolean_set_operations_2/do_intersect.cpp

// Q: Should we be using CGAL::Polyhedron_3?
// A: No! Input is just a list of unoriented triangles. Polyhedron_3 requires
// a 2-manifold.
// A: But! It seems we could use CGAL::Triangulation_3. Though it won't be easy
// to take advantage of functions like insert_in_facet because we want to
// constrain segments. Hmmm. Actualy Triangulation_3 doesn't look right...

//static void box_intersect(SelfIntersectMesh * SIM,const Box & A, const Box & B)
//{
//  return SIM->box_intersect(A,B);
//}


// CGAL's box_self_intersection_d uses C-style function callbacks without
// userdata. This is a leapfrog method for calling a member function. It should
// be bound as if the prototype was:
//   static void box_intersect(const Box &a, const Box &b)
// using boost:
//  boost::function<void(const Box &a,const Box &b)> cb
//    = boost::bind(&::box_intersect, this, _1,_2);
//   
static void box_intersect(SelfIntersectMesh * SIM, const Box &a, const Box &b)
{
  SIM->box_intersect(a,b);
}

#define FIRST_HIT_EXCEPTION 10

SelfIntersectMesh::SelfIntersectMesh(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  const SelfintersectParam & params,
  Eigen::MatrixXd & VV,
  Eigen::MatrixXi & FF):
  V(V),
  F(F),
  count(0),
  F_objects(F.rows()),
  T(),
  lIF(),
  offensive(F.rows(),false),
  offending_index(F.rows(),-1),
  offending(),
  edge2faces(),
  params(params)
{
  using namespace std;
  using namespace Eigen;
  // Compute and process self intersections
  mesh_to_triangles(V,F,T);
  // http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Box_intersection_d/Chapter_main.html#Section_63.5 
  // Create the corresponding vector of bounding boxes
  std::vector<Box> boxes;
  boxes.reserve(T.size());
  for ( 
    TrianglesIterator tit = T.begin(); 
    tit != T.end(); 
    ++tit)
  {
    boxes.push_back(Box(tit->bbox(), tit));
  }
  // Leapfrog callback
  boost::function<void(const Box &a,const Box &b)> cb
    = boost::bind(&::box_intersect, this, _1,_2);
  // Run the self intersection algorithm with all defaults
  try{
    CGAL::box_self_intersection_d(boxes.begin(), boxes.end(),cb);
  }catch(int e)
  {
    // Rethrow if not FIRST_HIT_EXCEPTION
    if(e != FIRST_HIT_EXCEPTION)
    {
      throw e;
    }
    // Otherwise just fall through
  }

  if(params.detect_only)
  {
    return;
  }
#ifdef INEXACT_CONSTRUCTION
  mexErrMsgTxt("'DectectOnly' must be set to true, "
      "or recompile without -DINEXACT_CONSTRUCTION");
#endif

  int NF_count = 0;
  // list of new faces, we'll fix F later
  vector<MatrixXi> NF(offending.size());
  // list of new vertices
  list<Point_3> NV;
  int NV_count = 0;
  vector<CDT_plus_2> cdt(offending.size());
  vector<Plane_3> P(offending.size());
  // Use map for *all* faces
  map<CDT_plus_2::Vertex_handle,int> v2i;
  // Loop over offending triangles
  for(int o = 0;o<(int)offending.size();o++)
  {
    // index in F
    const int f = offending[o];
    projected_delaunay(T[f],F_objects[f],cdt[o]);
    // Q: Is this also delaunay in 3D?
    // A: No, because the projection is affine and delaunay is not affine
    // invariant.
    // Q: Then, can't we first get the 2D delaunay triangulation, then lift it to 3D
    // and flip any offending edges?
    // Plane of projection (also used by projected_delaunay)
    P[o] = Plane_3(T[f].vertex(0),T[f].vertex(1),T[f].vertex(2));
    // Build index map
    {
      int i=0;
      for(
        CDT_plus_2::Finite_vertices_iterator vit = cdt[o].finite_vertices_begin();
        vit != cdt[o].finite_vertices_end();
        ++vit)
      {
        if(i<3)
        {
          //cout<<T[f].vertex(i)<<
          //  (T[f].vertex(i) == P[o].to_3d(vit->point())?" == ":" != ")<<
          //  P[o].to_3d(vit->point())<<endl;
#ifndef NDEBUG
          // I want to be sure that the original corners really show up as the
          // original corners of the CDT. I.e. I don't trust CGAL to maintain the
          // order
          assert(T[f].vertex(i) == P[o].to_3d(vit->point()));
#endif
          // For first three, use original index in F
          v2i[vit] = F(f,i);
        }else
        {
          const Point_3 vit_point_3 = P[o].to_3d(vit->point());
          // First look up each edge's neighbors to see if exact point has
          // already been added (This makes everything a bit quadratic)
          bool found = false;
          for(int e = 0; e<3 && !found;e++)
          {
            // Index of F's eth edge in V
            int i = F(f,(e+1)%3);
            int j = F(f,(e+2)%3);
            const Point_3 pi(V(i,0), V(i,1), V(i,2));
            const Point_3 pj(V(j,0), V(j,1), V(j,2));
            // Skip if vit_point_3 is not on the edge (pi, pj)
            if (!collinear(vit_point_3, pi, pj)) continue;
            // Be sure that i<j
            if(i>j)
            {
              swap(i,j);
            }
            assert(edge2faces.count(EMK(i,j))==1);
            // loop over neighbors
            for(
              list<int>::const_iterator nit = edge2faces[EMK(i,j)].begin();
              nit != edge2faces[EMK(i,j)].end() && !found;
              nit++)
            {
              // don't consider self
              if(*nit == f)
              {
                continue;
              }
              // index of neighbor in offending (to find its cdt)
              int no = offending_index[*nit];
              // Loop over vertices of that neighbor's cdt (might not have been
              // processed yet, but then it's OK because it'll just be empty)
              for(
                  CDT_plus_2::Finite_vertices_iterator uit = cdt[no].finite_vertices_begin();
                  uit != cdt[no].finite_vertices_end() && !found;
                  ++uit)
              {
                if(vit_point_3 == P[no].to_3d(uit->point()))
                {
                  assert(v2i.count(uit) == 1);
                  v2i[vit] = v2i[uit];
                  found = true;
                }
              }
            }
          }
          if(!found)
          {
            v2i[vit] = V.rows()+NV_count;
            NV.push_back(vit_point_3);
            NV_count++;
          }
        }
        i++;
      }
    }
    {
      int i = 0;
      // Resize to fit new number of triangles
      NF[o].resize(cdt[o].number_of_faces(),3);
      NF_count+=NF[o].rows();
      // Append new faces to NF
      for(
        CDT_plus_2::Finite_faces_iterator fit = cdt[o].finite_faces_begin();
        fit != cdt[o].finite_faces_end();
        ++fit)
      {
        NF[o](i,0) = v2i[fit->vertex(0)];
        NF[o](i,1) = v2i[fit->vertex(1)];
        NF[o](i,2) = v2i[fit->vertex(2)];
        i++;
      }
    }
  }
  assert(NV_count == (int)NV.size());
  // Build output
#ifndef NDEBUG
  {
    int off_count = 0;
    for(int f = 0;f<F.rows();f++)
    {
      off_count+= (offensive[f]?1:0);
    }
    assert(off_count==(int)offending.size());
  }
#endif
  // Append faces
  FF.resize(F.rows()-offending.size()+NF_count,3);
  // First append non-offending original faces
  // There's an Eigen way to do this in one line but I forget
  int off = 0;
  for(int f = 0;f<F.rows();f++)
  {
    if(!offensive[f])
    {
      FF.row(off++) = F.row(f);
    }
  }
  assert(off == (int)(F.rows()-offending.size()));
  // Now append replacement faces for offending faces
  for(int o = 0;o<(int)offending.size();o++)
  {
    FF.block(off,0,NF[o].rows(),3) = NF[o];
    off += NF[o].rows();
  }
  // Append vertices
  VV.resize(V.rows()+NV_count,3);
  VV.block(0,0,V.rows(),3) = V;
  {
    int i = 0;
    for(
      list<Point_3>::const_iterator nvit = NV.begin();
      nvit != NV.end();
      nvit++)
    {
      for(int d = 0;d<3;d++)
      {
        const Point_3 & p = *nvit;
#ifdef INEXACT_CONSTRUCTION
        VV(V.rows()+i,d) = CGAL::to_double(p[d]);
#else
        VV(V.rows()+i,d) = CGAL::to_double(p[d].exact());
#endif
      }
      i++;
    }
  }

  // Q: Does this give the same result as TETGEN?
  // A: For the cow and beast, yes.

  // Q: Is tetgen faster than this CGAL implementation?
  // A: Well, yes. But Tetgen is only solving the detection (predicates)
  // problem. This is also appending the intersection objects (construction).
  // But maybe tetgen is still faster for the detection part. For example, this
  // CGAL implementation on the beast takes 98 seconds but tetgen detection
  // takes 14 seconds

}

void SelfIntersectMesh::mesh_to_triangles(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  Triangles & T)
{
  // Must be 3D
  assert(V.cols() == 3);
  // Must be triangles
  assert(F.cols() == 3);
  T.reserve(F.rows());
  // Loop over faces
  for(int f = 0;f<(int)F.rows();f++)
  {
    T.push_back(
      Triangle_3(
        Point_3( V(F(f,0),0), V(F(f,0),1), V(F(f,0),2)),
        Point_3( V(F(f,1),0), V(F(f,1),1), V(F(f,1),2)),
        Point_3( V(F(f,2),0), V(F(f,2),1), V(F(f,2),2))));
  }
}

void SelfIntersectMesh::mark_offensive(const int f)
{
  using namespace std;
  lIF.push_back(f);
  if(!offensive[f])
  {
    offensive[f]=true;
    offending_index[f]=offending.size();
    offending.push_back(f);
    // Add to edge map
    for(int e = 0; e<3;e++)
    {
      // Index of F's eth edge in V
      int i = F(f,(e+1)%3);
      int j = F(f,(e+2)%3);
      // Be sure that i<j
      if(i>j)
      {
        swap(i,j);
      }
      // Create new list if there is no entry
      if(edge2faces.count(EMK(i,j))==0)
      {
        edge2faces[EMK(i,j)] = list<int>();
      }
      // append to list
      edge2faces[EMK(i,j)].push_back(f);
    }
  }
}

void SelfIntersectMesh::count_intersection(const int fa,const int fb)
{
  mark_offensive(fa);
  mark_offensive(fb);
  this->count++;
  // We found the first intersection
  if(params.first_only && this->count >= 1)
  {
    throw FIRST_HIT_EXCEPTION;
  }
}

bool SelfIntersectMesh::intersect(
  const Triangle_3 & A, 
  const Triangle_3 & B, 
  const int fa,
  const int fb)
{
  // Determine whether there is an intersection
  if(!CGAL::do_intersect(A,B))
  {
    return false;
  }
  if(!params.detect_only)
  {
    // Construct intersection
    CGAL::Object result = CGAL::intersection(A,B);
    F_objects[fa].push_back(result);
    F_objects[fb].push_back(result);
  }
  count_intersection(fa,fb);
  return true;
}

bool SelfIntersectMesh::single_shared_vertex(
  const Triangle_3 & A,
  const Triangle_3 & B,
  const int fa,
  const int fb,
  const int va,
  const int vb)
{
  ////using namespace std;
  //CGAL::Object result = CGAL::intersection(A,B);
  //if(CGAL::object_cast<Segment_3 >(&result))
  //{
  //  // Append to each triangle's running list
  //  F_objects[fa].push_back(result);
  //  F_objects[fb].push_back(result);
  //  count_intersection(fa,fb);
  //}else
  //{
  //  // Then intersection must be at point
  //  // And point must be at shared vertex
  //  assert(CGAL::object_cast<Point_3>(&result));
  //}
  //
  //if(single_shared_vertex(A,B,fa,fb,va))
  //{
  //  return true;
  //}
  //return single_shared_vertex(B,A,fb,fa,vb);
  bool r1 = single_shared_vertex(A,B,fa,fb,va);
  bool r2 = single_shared_vertex(B,A,fb,fa,vb);
  return r1 || r2;
}

bool SelfIntersectMesh::single_shared_vertex(
  const Triangle_3 & A,
  const Triangle_3 & B,
  const int fa,
  const int fb,
  const int va)
{
  using namespace std;
  Segment_3 sa(
    A.vertex((va+1)%3),
    A.vertex((va+2)%3));
  CGAL::Object result = CGAL::intersection(sa,B);
  if(const Point_3 * p = CGAL::object_cast<Point_3 >(&result))
  {
    if(!params.detect_only)
    {
      // Single intersection --> segment from shared point to intersection
      CGAL::Object seg = CGAL::make_object(Segment_3(
        A.vertex(va),
        *p));
      F_objects[fa].push_back(seg);
      F_objects[fb].push_back(seg);
    }
    count_intersection(fa,fb);
    return true;
  }else if(const Segment_3 * s = CGAL::object_cast<Segment_3 >(&result))
  {
    if(!params.detect_only)
    {
      // Segment intersection --> triangle from shared point to intersection
      CGAL::Object tri = CGAL::make_object(Triangle_3(
        A.vertex(va),
        s->vertex(0),
        s->vertex(1)));
      F_objects[fa].push_back(tri);
      F_objects[fb].push_back(tri);
    }
    count_intersection(fa,fb);
    cerr<<REDRUM("Coplanar at: "<<fa<<" & "<<fb<<".")<<endl;
    return true;
  }

  return false;
}

bool SelfIntersectMesh::double_shared_vertex(
        const Triangle_3 & A,
        const Triangle_3 & B,
        const int fa,
        const int fb,
        const int va1,
        const int vb1,
        const int va2,
        const int vb2
        ) {
    int va3=-1;
    int vb3=-1;
    for (int i=0; i<3; i++) {
        if (i != va1 && i != va2)
            va3 = i;
        if (i != vb1 && i != vb2)
            vb3 = i;
    }

    Segment_3 ea_13(A.vertex(va1), A.vertex(va3));
    Segment_3 ea_23(A.vertex(va2), A.vertex(va3));
    Segment_3 eb_13(B.vertex(vb1), B.vertex(vb3));
    Segment_3 eb_23(B.vertex(vb2), B.vertex(vb3));
    CGAL::Object result_1 = CGAL::intersection(ea_13, eb_23);
    CGAL::Object result_2 = CGAL::intersection(ea_23, eb_13);
    CGAL::Object result_3 = CGAL::intersection(ea_13, eb_13);
    CGAL::Object result_4 = CGAL::intersection(ea_23, eb_23);

    if (const Segment_3* seg = CGAL::object_cast<Segment_3>(&result_3)) {
        std::cerr << "oh crap!!" << std::endl;
    }
    if (const Segment_3* seg = CGAL::object_cast<Segment_3>(&result_4)) {
        std::cerr << "oh crap!" << std::endl;
    }

    if (const Point_3* p = CGAL::object_cast<Point_3>(&result_1)) {
        if (!params.detect_only) {
            CGAL::Object tri = CGAL::make_object(
                    Triangle_3(A.vertex(va1), A.vertex(va2), *p));
            F_objects[fa].push_back(tri);
            F_objects[fb].push_back(tri);
        }
        count_intersection(fa, fb);
        return true;
    } else if (const Segment_3* seg = CGAL::object_cast<Segment_3>(&result_1)){
        std::cerr << "One of the triangles must be degenerated." << std::endl;
    }
    if (const Point_3* p = CGAL::object_cast<Point_3>(&result_2)) {
        if (!params.detect_only) {
            CGAL::Object tri = CGAL::make_object(
                    Triangle_3(A.vertex(va1), A.vertex(va2), *p));
            F_objects[fa].push_back(tri);
            F_objects[fb].push_back(tri);
        }
        count_intersection(fa, fb);
        return true;
    } else if (const Segment_3* seg = CGAL::object_cast<Segment_3>(&result_2)){
        std::cerr << "One of the triangles must be degenerated." << std::endl;
    }
    return false;
}

void SelfIntersectMesh::box_intersect(const Box& a, const Box& b)
{
  using namespace std;
  // index in F and T
  int fa = a.handle()-T.begin();
  int fb = b.handle()-T.begin();
  const Triangle_3 & A = *a.handle();
  const Triangle_3 & B = *b.handle();
  // I'm not going to deal with degenerate triangles, though at some point we
  // should
  if (a.handle()->is_degenerate() || b.handle()->is_degenerate()) {
      cerr << "degenerated triangle" << std::endl;
      return;
  }
  assert(!a.handle()->is_degenerate());
  assert(!b.handle()->is_degenerate());
  // Number of combinatorially shared vertices
  int comb_shared_vertices = 0;
  // Number of geometrically shared vertices (*not* including combinatorially
  // shared)
  int geo_shared_vertices = 0;
  // Keep track of shared vertex indices (we only handles single shared
  // vertices as a special case, so just need last/first/only ones)
  int va[3] = {-1, -1, -1};
  int vb[3] = {-1, -1, -1};
  size_t share_count=0;
  int ea,eb;
  for(ea=0;ea<3;ea++)
  {
    for(eb=0;eb<3;eb++)
    {
      if(F(fa,ea) == F(fb,eb))
      {
        comb_shared_vertices++;
        va[share_count] = ea;
        vb[share_count] = eb;
        share_count++;
      }else if(A.vertex(ea) == B.vertex(eb))
      {
        geo_shared_vertices++;
        va[share_count] = ea;
        vb[share_count] = eb;
        share_count++;
      }
    }
  }
  const int total_shared_vertices = comb_shared_vertices + geo_shared_vertices;
  if(geo_shared_vertices== 3)
  {
    // Combinatorially duplicate face, these should be removed by preprocessing
    cerr<<REDRUM("Facets "<<fa<<" and "<<fb<<" are combinatorial duplicates")<<endl;
    goto done;
  }
  if(total_shared_vertices== 3)
  {
    // Geometrically duplicate face, these should be removed by preprocessing
    cerr<<REDRUM("Facets "<<fa<<" and "<<fb<<" are geometrical duplicates")<<endl;
    goto done;
  }
  if(total_shared_vertices == 2)
  {
    // Single shared edge --> no intersection
    assert(va[0] >= 0 && va[0] < 3);
    assert(vb[0] >= 0 && vb[0] < 3);
    assert(va[1] >= 0 && va[1] < 3);
    assert(vb[1] >= 0 && vb[1] < 3);
    double_shared_vertex(A,B,fa,fb,
            va[0],vb[0], va[1], vb[1]);
  }
  //assert(total_shared_vertices<=1);
  if(total_shared_vertices==1)
  {
    //assert(va>=0 && va<3);
    //assert(vb>=0 && vb<3);
//#ifndef NDEBUG
//    CGAL::Object result =
//#endif
    assert(va[0] >= 0 && va[0] < 3);
    assert(vb[0] >= 0 && vb[0] < 3);
    single_shared_vertex(A,B,fa,fb,va[0],vb[0]);
    //single_shared_vertex(A,B,fa,fb,va,vb);
//#ifndef NDEBUG
//    if(!CGAL::object_cast<Segment_3 >(&result))
//    {
//      const Point_3 * p = CGAL::object_cast<Point_3 >(&result);
//      assert(p);
//      for(int ea=0;ea<3;ea++)
//      {
//        for(int eb=0;eb<3;eb++)
//        {
//          if(F(fa,ea) == F(fb,eb))
//          {
//            assert(*p==A.vertex(ea));
//            assert(*p==B.vertex(eb));
//          }
//        }
//      }
//    }
//#endif
  }else
  {
    // No geometrically shared vertices, do general intersect
    intersect(*a.handle(),*b.handle(),fa,fb);
  }
done:
  return;
}

// Compute 2D delaunay triangulation of a given 3d triangle and a list of
// intersection objects (points,segments,triangles). CGAL uses an affine
// projection rather than an isometric projection, so we're not guaranteed
// that the 2D delaunay triangulation here will be a delaunay triangulation
// in 3D.
//
// Inputs:
//   A  triangle in 3D
//   A_objects_3  updated list of intersection objects for A
// Outputs:
//   cdt  Contrained delaunay triangulation in projected 2D plane
void SelfIntersectMesh::projected_delaunay(
  const Triangle_3 & A,
  const std::list<CGAL::Object> & A_objects_3,
  CDT_plus_2 & cdt)
{
  using namespace std;
  typedef std::vector<Point_3> PointVec;
  // http://www.cgal.org/Manual/3.2/doc_html/cgal_manual/Triangulation_2/Chapter_main.html#Section_2D_Triangulations_Constrained_Plus
  // Plane of triangle A
  Plane_3 P(A.vertex(0),A.vertex(1),A.vertex(2));
  // Insert triangle into vertices
  CDT_plus_2::Vertex_handle corners[3];
  for(int i = 0;i<3;i++)
  {
    corners[i] = cdt.insert(P.to_2d(A.vertex(i)));
  }
  // Insert triangle edges as constraints
  for(int i = 0;i<3;i++)
  {
    cdt.insert_constraint( corners[(i+1)%3], corners[(i+2)%3]);
  }
  // Insert constraints for intersection objects
  for(
    list<CGAL::Object>::const_iterator lit = A_objects_3.begin();
    lit != A_objects_3.end();
    lit++)
  {
    CGAL::Object obj = *lit;
    if(const Point_3 *ipoint = CGAL::object_cast<Point_3 >(&obj))
    {
      // Add point
      cdt.insert(P.to_2d(*ipoint));
    } else if(const Segment_3 *iseg = CGAL::object_cast<Segment_3 >(&obj))
    {
      // Add segment constraint
      cdt.insert_constraint(P.to_2d(iseg->vertex(0)),P.to_2d(iseg->vertex(1)));
    } else if(const Triangle_3 *itri = CGAL::object_cast<Triangle_3 >(&obj))
    {
      // Add 3 segment constraints
      cdt.insert_constraint(P.to_2d(itri->vertex(0)),P.to_2d(itri->vertex(1)));
      cdt.insert_constraint(P.to_2d(itri->vertex(1)),P.to_2d(itri->vertex(2)));
      cdt.insert_constraint(P.to_2d(itri->vertex(2)),P.to_2d(itri->vertex(0)));
    } else if (const PointVec* ipvec = CGAL::object_cast<PointVec>(&obj))
    {
        // Handle coplanar triangle intersection that is a polygon.
        for (PointVec::const_iterator itr = ipvec->begin(); itr != ipvec->end(); itr++) {
            PointVec::const_iterator jtr = itr;
            jtr++;
            if (jtr == ipvec->end())
                jtr=ipvec->begin();
            const Point_3& p = *itr;
            cdt.insert_constraint(P.to_2d(*itr), P.to_2d(*jtr));
        }
    }else
    {
      cerr<<"What the fuck is this object?!"<<endl;
      assert(false);
    }
  }
}
