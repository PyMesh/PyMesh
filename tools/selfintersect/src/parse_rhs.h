#ifndef PARSE_RHS_H
#define PARSE_RHS_H

#include <mex.h>
#include <Eigen/Core>
// Parse right hand side arguments for a matlab mex function.
//
// Inputs:
//   nrhs  number of right hand side arguments
//   prhs  pointer to right hand side arguments
// Outputs:
//   V  n by dim list of mesh vertex positions
//   F  m by dim list of mesh face indices
// Throws matlab errors if dimensions are not sane.
void parse_rhs(
  const int nrhs, 
  const mxArray *prhs[], 
  Eigen::MatrixXd & V,
  Eigen::MatrixXi & F);
#endif 
