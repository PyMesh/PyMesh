#ifndef SELFINTERSECTMESH_H
#define SELFINTERSECTMESH_H

#include "CGAL_includes.hpp"
#include "selfintersect.h"

#include <Eigen/Dense>
#include <list>
#include <map>
#include <vector>


// The easiest way to keep track of everything is to use a class
class SelfIntersectMesh
{
  private:
    // Input mesh
    const Eigen::MatrixXd & V;
    const Eigen::MatrixXi & F;
    // Number of self-intersecting triangle pairs
    int count;
    std::vector<std::list<CGAL::Object> > F_objects;
    Triangles T;
    std::list<int> lIF;
    std::vector<bool> offensive;
    std::vector<int> offending_index;
    std::vector<int> offending;
    // Make a short name for the edge map's key
    typedef std::pair<int,int> EMK;
    // Make a short name for the type stored at each edge, the edge map's value
    typedef std::list<int> EMV;
    // Make a short name for the edge map
    typedef std::map<EMK,EMV> EdgeMap;
    EdgeMap edge2faces;
  public:
    SelfintersectParam params;
  public:
    // Constructs (VV,FF) a new mesh with self-intersections of (V,F)
    // subdivided
    SelfIntersectMesh(
      const Eigen::MatrixXd & V,
      const Eigen::MatrixXi & F,
      const SelfintersectParam & params,
      Eigen::MatrixXd & VV,
      Eigen::MatrixXi & FF);
  private:
    // Convert a mesh (V,F) to a list of CGAL triangles
    //
    // Inputs:
    //   V  #V by 3 list of vertex positions
    //   F  #F by 3 list of triangle indices
    // Outputs:
    //   T  #F list of CGAL triangles
    static void mesh_to_triangles(
      const Eigen::MatrixXd & V,
      const Eigen::MatrixXi & F,
      Triangles & T);
    // Helper function to mark a face as offensive
    //
    // Inputs:
    //   f  index of face in F
    void mark_offensive(const int f);
    // Helper function to count intersections between faces
    //
    // Input:
    //   fa  index of face A in F
    //   fb  index of face B in F
    void count_intersection(const int fa,const int fb);
    // Helper function for box_intersect. Intersect two triangles A and B, append
    // the intersection object (point,segment,triangle) to a running list for A and
    // B
    //
    // Inputs:
    //   A  triangle in 3D
    //   B  triangle in 3D
    //   fa  index of A in F (and F_objects)
    //   fb  index of A in F (and F_objects)
    // Returns true only if A intersects B
    //
    bool intersect(
        const Triangle_3 & A, 
        const Triangle_3 & B, 
        const int fa,
        const int fb);
    // Helper function for box_intersect. In the case where A and B have already
    // been identified to share a vertex, then we only want to add possible segment
    // intersections. Assumes truly duplicate triangles are not given as input
    //
    // Inputs:
    //   A  triangle in 3D
    //   B  triangle in 3D
    //   fa  index of A in F (and F_objects)
    //   fb  index of B in F (and F_objects)
    //   va  index of shared vertex in A (and F_objects)
    //   vb  index of shared vertex in B (and F_objects)
    //// Returns object of intersection (should be Segment or point)
    //   Returns true if intersection (besides shared point)
    //
    bool single_shared_vertex(
        const Triangle_3 & A,
        const Triangle_3 & B,
        const int fa,
        const int fb,
        const int va,
        const int vb);
    // Helper handling one direction
    bool single_shared_vertex(
        const Triangle_3 & A,
        const Triangle_3 & B,
        const int fa,
        const int fb,
        const int va);

  public:
    // Callback function called during box self intersections test. Means boxes
    // a and b intersect. This method then checks if the triangles in each box
    // intersect and if so, then processes the intersections
    //
    // Inputs:
    //   a  box containing a triangle
    //   b  box containing a triangle
    void box_intersect(const Box& a, const Box& b);
  private:
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
    void projected_delaunay(
        const Triangle_3 & A,
        const std::list<CGAL::Object> & A_objects_3,
        CDT_plus_2 & cdt);
    // Getters:
  public:
    const std::list<int>& get_lIF() const{ return lIF;}
};
#endif
