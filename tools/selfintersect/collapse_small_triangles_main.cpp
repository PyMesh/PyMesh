#include "collapse_small_triangles.h"
#ifdef MEX
#  include "parse_rhs.h"
#endif

#include <igl/read.h>
#include <igl/EPS.h>
#include <igl/pathinfo.h>
#include <igl/writeOFF.h>
#include <igl/writeOBJ.h>
#include <igl/writeDMAT.h>
#include <igl/removeDuplicates.h>
#ifdef MEX
#  define IGL_REDRUM_NOOP
#endif
#include <igl/REDRUM.h>
#ifdef MEX
#  include <igl/matlab/mexStream.h>
#endif

#ifdef MEX
#  include "mex.h"
#endif
#include <iostream>
#include <string>

#ifdef MEX
void mexFunction(int nlhs, mxArray *plhs[], 
    int nrhs, const mxArray *prhs[])
{
  //mexPrintf("Compiled at %s on %s\n",__TIME__,__DATE__);
  if(nrhs != 2 && nrhs != 3)
  {
    mexErrMsgTxt("The number of input arguments must be 2 or 3.");
  }
  //mexPrintf("%s %s\n",__TIME__,__DATE__);
  igl::mexStream mout;
  std::streambuf *outbuf = std::cout.rdbuf(&mout);

#else
int main(int argc, char * argv[])
{
#endif
  using namespace std;
  using namespace Eigen;
  using namespace igl;

  MatrixXd V;
  MatrixXi F;
  VectorXi IM;

  string prefix;
  bool use_obj_format = false;
  double eps = FLOAT_EPS;
#ifdef MEX
  // This parses first two arguements
  parse_rhs(nrhs,prhs,V,F);
  if(nrhs==3)
  {
    if(mxGetM(prhs[2]) != 1 || mxGetN(prhs[2]) != 1)
    {
      mexErrMsgTxt("3rd argument should be scalar");
    }
    eps = *mxGetPr(prhs[2]);
  }
#else
  if(argc <= 1 || argc>3)
  {
    cerr<<"Usage:"<<endl<<
      "  collapse_small_triangles [path to mesh]"<<endl<<
      "or "<<endl<<
      "  collapse_small_triangles [path to mesh] [eps]"<<endl;
    return 1;
  }
  // Load mesh
  string filename(argv[1]);
  if(!read(filename,V,F))
  {
    cout<<REDRUM("Reading "<<filename<<" failed.")<<endl;
    return false;
  }
  cout<<GREENGIN("Read "<<filename<<" successfully.")<<endl;
  {
    // dirname, basename, extension and filename
    string dirname,b,ext;
    pathinfo(filename,dirname,b,ext,prefix);
    prefix = dirname + "/" + prefix;
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    use_obj_format = ext == "obj";
  }
  if(argc == 3)
  {
    if(sscanf(argv[2], "%lf", &eps)!=1)
    {
      cout<<REDRUM("Parsing "<<argv[2]<<" failed.")<<endl;
      return 1;
    }else
    {
      cout<<"eps: "<<eps<<endl;
    }
  }else
  {
    cout<<"Default eps: "<<eps<<endl;
  }
#endif


  // let's first try to merge small triangles
  {
    MatrixXd tempV;
    MatrixXi tempF;
    collapse_small_triangles(V,F,eps,tempF);
    //if(F.rows() == tempF.rows())
    //{
    //  cout<<GREENGIN("No small triangles detected.")<<endl;
    //}else
    //{
    //  cout<<BLUEGIN("Collapsed all small triangles, reducing "<<F.rows()<<
    //    " input triangles to "<<tempF.rows()<<" triangles.")<<endl;
    //}
    F=tempF;
#ifndef MEX
    if(use_obj_format)
    {
      cout<<"writing mesh to "<<(prefix+"-collapse.obj")<<endl;
      writeOBJ(prefix+"-collapse.obj",V,F);
    }else
    {
      cout<<"writing mesh to "<<(prefix+"-collapse.off")<<endl;
      writeOFF(prefix+"-collapse.off",V,F);
    }
#endif
  }

#ifdef MEX
  // Prepare left-hand side
  nlhs = 1;

  // Treat indices as reals
  plhs[0] = mxCreateDoubleMatrix(F.rows(),F.cols(), mxREAL);
  double * Fp = mxGetPr(plhs[0]);
  MatrixXd Fd = (F.cast<double>().array()+1).matrix();
  copy(&Fd.data()[0],&Fd.data()[0]+Fd.size(),Fp);

  // Restore the std stream buffer Important!
  std::cout.rdbuf(outbuf);

#else
  return 0;
#endif
}


