#ifdef MEX
#include "prepare_lhs.h"

void prepare_lhs(
  const int nlhs,
  mxArray *plhs[],
  const int no,
  double *& S)
{
  // Create pointers for output arrays
  if(nlhs > 0)
  {
    // solid angles
    plhs[0] = mxCreateDoubleMatrix(no,1, mxREAL);
    S = mxGetPr(plhs[0]);
  }
}
#endif
