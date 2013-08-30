#include "selfintersect.h"
#ifdef MEX
#  include "parse_rhs.h"
#endif

// IGL LIB includes
#include <igl/bounding_box_diagonal.h>
#include <igl/read.h>
#include <igl/pathinfo.h>
#include <igl/writeOFF.h>
#include <igl/writeOBJ.h>
#include <igl/writeDMAT.h>
#include <igl/removeDuplicates.h>
#include <igl/unique_simplices.h>
#include <igl/C_STR.h>
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
  // This is useful for debugging whether Matlab is caching the mex binary
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
  SelfintersectParam params;

  string prefix;
  bool use_obj_format = false;
#ifdef MEX
  parse_rhs(nrhs,prhs,V,F);
  if(nrhs>2)
  {
    int i = 2;
    while(i<nrhs)
    {
      if(!mxIsChar(prhs[i]))
      {
        mexErrMsgTxt("Parameter names should be char strings");
      }
      // Cast to char
      const char * name = mxArrayToString(prhs[i]);
      if(strcmp("DetectOnly",name) == 0)
      {
        if((i+1)>=nrhs)
        {
          mexErrMsgTxt(C_STR("Parameter '"<<name<<"' requires argument"));
        }
        i++;
        if(!mxIsLogical(prhs[i]))
        {
          mexErrMsgTxt(C_STR("Parameter '"<<name<<"' requires Logical arg"));
        }
        mxLogical * v = (mxLogical *)mxGetData(prhs[i]);
        params.detect_only = *v;
      }else if(strcmp("FirstOnly",name) == 0)
      {
        if((i+1)>=nrhs)
        {
          mexErrMsgTxt(C_STR("Parameter '"<<name<<"' requires argument"));
        }
        i++;
        if(!mxIsLogical(prhs[i]))
        {
          mexErrMsgTxt(C_STR("Parameter '"<<name<<"' requires Logical arg"));
        }
        mxLogical * v = (mxLogical *)mxGetData(prhs[i]);
        params.first_only = *v;
      }else
      {
        mexErrMsgTxt(C_STR("Unsupported parameter: "<<name));
      }
      i++;
    }
  }
#else
  if(argc <= 1)
  {
    cerr<<"Usage:"<<endl<<"  selfintersect [path to .off/.obj mesh]"<<endl;
    return 1;
  }
  // Apparently CGAL doesn't have a good data structure triangle soup. Their
  // own examples use (V,F):
  // http://www.cgal.org/Manual/latest/doc_html/cgal_manual/AABB_tree/Chapter_main.html#Subsection_64.3.7
  //
  // Load mesh
  string filename(argv[1]);
  if(!read(filename,V,F))
  {
    //cout<<REDRUM("Reading "<<filename<<" failed.")<<endl;
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
#endif

  // Now mesh self intersections
  MatrixXi IF;
  {
    MatrixXd tempV;
    MatrixXi tempF;
    selfintersect(V,F,params,tempV,tempF,IF);
    //cout<<BLUEGIN("Found and meshed "<<IF.rows()<<" pair"<<(IF.rows()==1?"":"s")
    //  <<" of self-intersecting triangles.")<<endl;
    V=tempV;
    F=tempF;
#ifndef MEX
    cout<<"writing pair list to "<<(prefix+"-IF.dmat")<<endl;
    writeDMAT((prefix+"-IF.dmat").c_str(),IF);
    if(use_obj_format)
    {
      cout<<"writing mesh to "<<(prefix+"-selfintersect.obj")<<endl;
      writeOBJ(prefix+"-selfintersect.obj",V,F);
    }else
    {
      cout<<"writing mesh to "<<(prefix+"-selfintersect.off")<<endl;
      writeOFF(prefix+"-selfintersect.off",V,F);
    }
#endif
  }

  // Double-check output

#ifdef DEBUG
  // There should be *no* combinatorial duplicates
  {
    MatrixXi tempF;
    unique_simplices(F,tempF);
    if(tempF.rows() < F.rows())
    {
      cout<<REDRUM("Error: selfintersect created "<<
        F.rows()-tempF.rows()<<" combinatorially duplicate faces")<<endl;
    }else
    {
      assert(tempF.rows() == F.rows());
      cout<<GREENGIN("selfintersect created no duplicate faces")<<endl;
    }
    F = tempF;
  }
#endif

#ifdef MEX
  // Prepare left-hand side
  nlhs = 3;

  plhs[0] = mxCreateDoubleMatrix(V.rows(),V.cols(), mxREAL);
  double * Vp = mxGetPr(plhs[0]);
  copy(&V.data()[0],&V.data()[0]+V.size(),Vp);

  // Treat indices as reals
  plhs[1] = mxCreateDoubleMatrix(F.rows(),F.cols(), mxREAL);
  double * Fp = mxGetPr(plhs[1]);
  MatrixXd Fd = (F.cast<double>().array()+1).matrix();
  copy(&Fd.data()[0],&Fd.data()[0]+Fd.size(),Fp);

  // Treat indices as reals
  plhs[2] = mxCreateDoubleMatrix(IF.rows(),IF.cols(), mxREAL);
  double * IFp = mxGetPr(plhs[2]);
  MatrixXd IFd = (IF.cast<double>().array()+1).matrix();
  copy(&IFd.data()[0],&IFd.data()[0]+IFd.size(),IFp);

  // Restore the std stream buffer Important!
  std::cout.rdbuf(outbuf);

#else
  return 0;
#endif
}
