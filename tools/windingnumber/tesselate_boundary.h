#ifndef TESSELATE_BOUNDARY_H
#define TESSELATE_BOUNDARY_H
#include <Eigen/Dense>
// Given a list of faces tesselate all of the "exterior" edges forming another
// list of 
//
// Inputs:
//   F  #F by simplex_size list of "faces"
// Outputs:
//   cap  #cap by simplex_size  list of "faces" tessleating the boundary edges
void tesselate_boundary(
  const Eigen::MatrixXi & F,
  Eigen::MatrixXi & cap);
// In-line version
Eigen::MatrixXi tesselate_boundary( const Eigen::MatrixXi & F);
#endif
