#ifdef MEX
#include "parse_rhs.h"
#include <algorithm>
#include <functional>

void parse_rhs(
  const int nrhs, 
  const mxArray *prhs[], 
  Eigen::MatrixXd & V,
  Eigen::MatrixXi & F)
{
  using namespace std;
  if(nrhs < 2)
  {
    mexErrMsgTxt("nrhs < 2");
  }

  const int dim = mxGetN(prhs[0]);
  if(dim != 3 && dim != 2)
  {
    mexErrMsgTxt("Mesh vertex list must be #V by 2 or 3 list of vertex positions");
  }

  if(dim != (int)mxGetN(prhs[1]))
  {
   mexErrMsgTxt("Mesh facet size must equal dimension");
  }

  // set number of mesh vertices
  const int n = mxGetM(prhs[0]);
  // set vertex position pointers
  double * Vp = mxGetPr(prhs[0]);

  // set number of faces
  const int m = mxGetM(prhs[1]);
  // set face index list pointer
  double * Fp = mxGetPr(prhs[1]);

  if((int)mxGetN(prhs[1]) != dim)
  {
    mexErrMsgTxt("Origin list dimension must match vertex list dimension");
  }

  // resize output to transpose
  V.resize(n,dim);
  copy(Vp,Vp+n*dim,&V.data()[0]);

  // resize output to transpose
  F.resize(m,dim);
  // Q: Is this doing a cast?
  // A: Yes.
  copy(Fp,Fp+m*dim,F.data());
  // http://stackoverflow.com/a/4461466/148668
  transform(F.data(),F.data()+m*dim,F.data(),
    bind2nd(std::plus<double>(),-1.0));
}
#endif
