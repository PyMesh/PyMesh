#ifndef PREPARE_LHS_H
#define PREPARE_LHS_H
#include <mex.h>
// Prepare left hand side
//
// Inputs:
//   nlhs  number of left hand side arguments
//   plhs  pointer to left hand side arguments
//   no  number of origins
// Outputs:
//   S  no by 1  list of solid angles
void prepare_lhs(
  const int nlhs,
  mxArray *plhs[],
  const int no,
  double *& S);

#endif
