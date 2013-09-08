#ifndef EXTERIOR_EDGES_H
#define EXTERIOR_EDGES_H
#include <Eigen/Dense>
// EXTERIOR_EDGES Determines boundary "edges" and also edges with an
// odd number of occurances where seeing edge (i,j) counts as +1 and seeing the
// opposite edge (j,i) counts as -1
//
// Inputs:
//   F  #F by simplex_size list of "faces"
// Outputs:
//   E  #E by simplex_size-1  list of exterior edges
//
void exterior_edges(
  const Eigen::MatrixXi & F,
  Eigen::MatrixXi & E);
// Inline version
Eigen::MatrixXi exterior_edges( const Eigen::MatrixXi & F);
#endif
