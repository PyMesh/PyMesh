#ifndef PARSE_RHS_H
#define PARSE_RHS_H
#include <mex.h>
// Struct to hold winding number parameters
struct winding_number_params
{
  // Whether to use hierarchical evaluation
  bool hierarchical;
};

// Parse right hand side arguments
//
// Inputs:
//   nrhs  number of right hand side arguments
//   prhs  pointer to right hand side arguments
// Outputs:
//   dim  number of dimensions == simplex size in F
//   V  n by dim list of mesh vertex positions
//   n  number of vertices
//   F  m by dim list of mesh face indices
//   m  number of faces
//   O  o by dim list of origins
//   no  number of origins
void parse_rhs(
  const int nrhs, 
  const mxArray *prhs[], 
  int & dim,
  double *& V,
  int & n,
  double *& F,
  int & m,
  double *& O,
  int & no,
  winding_number_params & params);
#endif

