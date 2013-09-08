#ifdef MEX
#include "parse_rhs.h"
#include <igl/C_STR.h>
#include <cstring>

// Overload mexErrMsgTxt to check an assertion then print text only if
// assertion fails
static void mexErrMsgTxt(bool assertion, const char * text)
{
  if(!assertion)
  {
    mexErrMsgTxt(text);
  }
}

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
  winding_number_params & params)
{
  using namespace std;
  mexErrMsgTxt(nrhs >= 3, "The number of input arguments must be >=3.");

  dim = mxGetN(prhs[0]);
  mexErrMsgTxt(dim == 3 || dim == 2,
    "Mesh vertex list must be #V by 2 or 3 list of vertex positions");

  mexErrMsgTxt(dim == mxGetN(prhs[1]),
    "Mesh \"face\" simplex size must equal dimension");

  // set number of mesh vertices
  n = mxGetM(prhs[0]);
  // set vertex position pointers
  V = mxGetPr(prhs[0]);

  // set number of faces
  m = mxGetM(prhs[1]);
  // set face index list pointer
  F = mxGetPr(prhs[1]);

  mexErrMsgTxt(mxGetN(prhs[2]) == dim, 
    "Origin list dimension must match vertex list dimension");
  // set number of faces
  no = mxGetM(prhs[2]);
  // set face index list pointer
  O = mxGetPr(prhs[2]);

  // Default values
  params.hierarchical = true;
  {
    int i = 3;
    while(i<nrhs)
    {
      mexErrMsgTxt(mxIsChar(prhs[i]),
        "Parameter names should be char strings");
      // Cast to char
      const char * name = mxArrayToString(prhs[i]);
      if(strcmp("Hierarchical",name) == 0)
      {
        mexErrMsgTxt((i+1)<nrhs,
          C_STR("Parameter '"<<name<<"' requires argument"));
        i++;
        mexErrMsgTxt(mxIsLogical(prhs[i]),
          C_STR("Parameter '"<<name<<"' requires Logical argument"));
        mxLogical * v = (mxLogical *)mxGetData(prhs[i]);
        params.hierarchical = *v;
        mexErrMsgTxt(!params.hierarchical || dim==3,
          "Hierarchical only supported for dim = 3");
      }else
      {
        mexErrMsgTxt(false,
          C_STR("Unsupported parameter: "<<name));
      }
      i++;
    }
  }
}

#endif
