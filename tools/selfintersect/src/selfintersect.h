#ifndef SELFINTERSECT_H
#define SELFINTERSECT_H

#include <Eigen/Dense>

// Optional Parameters
//   DetectOnly  Only compute IF, leave VV and FF alone
struct SelfintersectParam
{
  bool detect_only;
  bool first_only;
  SelfintersectParam():detect_only(false),first_only(false){};
};

// Given a triangle mesh (V,F) compute a new mesh (VV,FF) which is the same as
// (V,F) except that any self-intersecting triangles in (V,F) have been
// subdivided (new vertices and face created) so that the self-intersection
// contour lies exactly on edges in (VV,FF). New vertices will appear in
// original faces or on original edges. New vertices on edges are "merged" only
// across original faces sharing that edge. This means that if the input
// triangle mesh is a closed manifold the output will be too.
//
// Inputs:
//   V  #V by 3 list of vertex positions
//   F  #F by 3 list of triangle indices into V
//   params  struct of optional parameters
// Outputs:
//   VV  #VV by 3 list of vertex positions
//   FF  #FF by 3 list of triangle indices into V
//   IF  #intersecting face pairs by 2  list of intersecting face pairs,
//     indexing F
void selfintersect(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  const SelfintersectParam & params,
  Eigen::MatrixXd & VV,
  Eigen::MatrixXi & FF,
  Eigen::MatrixXi & IF);

void selfintersect_python(
        const Eigen::MatrixXd & V,
        const Eigen::MatrixXi & F,
        Eigen::MatrixXd & VV,
        Eigen::MatrixXi & FF);

class SelfIntersectionResolver {
    public:
        SelfIntersectionResolver(Eigen::MatrixXd& V, Eigen::MatrixXi& F) :
            m_vertices(V), m_faces(F) {}

    public:
        void resolve();
        Eigen::MatrixXd get_vertices() { return m_vertices; }
        Eigen::MatrixXi get_faces() { return m_faces; }
        Eigen::MatrixXi get_intersecting_faces() {
            return m_intersecting_faces;
        }
        Eigen::MatrixXi get_new_face_indices() {
            return m_new_face_idx;
        }

    private:
        Eigen::MatrixXd m_vertices;
        Eigen::MatrixXi m_faces;
        Eigen::MatrixXi m_intersecting_faces;
        Eigen::VectorXi m_new_face_idx;
};

#endif
