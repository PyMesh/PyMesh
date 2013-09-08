#ifdef MEX
// mex -v -largeArrayDims -DMEX ...
//   -I/usr/local/igl/igl_lib/include ...
//   -L/usr/local/igl/igl_lib/lib -ligl -liglmatlab ...
//   -I/opt/local/include/eigen3 ...
//   -o winding_number mexFunction.cpp ...
//   winding_number.cpp parse_rhs.cpp prepare_lhs.cpp ...
//   AABB.cpp BoundingTree.cpp tesselate_boundary.cpp exterior_edges.cpp ...
#include "winding_number.h"
#include "parse_rhs.h"
#include "prepare_lhs.h"
#include "AABB.h"

#include <igl/matlab/mexStream.h>
#include <igl/writeOBJ.h>
#include <igl/get_seconds.h>

#include <mex.h>
#include <Eigen/Dense>
#include <iostream>

void mexFunction(
  int nlhs, mxArray *plhs[], 
  int nrhs, const mxArray *prhs[])
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;

  igl::mexStream mout;        
  std::streambuf *outbuf = cout.rdbuf(&mout);
  //mexPrintf("Compiled at %s on %s\n",__TIME__,__DATE__);

  // vertex position list
  double * V;
  // face list
  double * Ftemp;
  // origin list
  double * O;
  // number of dimensions (and simplex size in F)
  int dim;
  // number of mesh vertices
  int n;
  // number of mesh faces
  int m;
  // number of origins
  int no;
  winding_number_params params;
  parse_rhs(nrhs,prhs,dim,V,n,Ftemp,m,O,no,params);

  double * F = new double[dim*m];
  // convert to 0-index
  //transform(Ftemp,Ftemp+m*dim,F,bind2nd(plus<double>(),-1.0));
  for(int i = 0;i<m;i++)
  {
    for(int d = 0;d<dim;d++)
    {
      F[d*m + i]=Ftemp[d*m+i]-1;
    }
  }

  //// Set up openmp
  //int nProcessors=omp_get_max_threads();
  ////std::cout<<nProcessors<<std::endl;
  //omp_set_num_threads(min(nProcessors,2));

  // List of winding numbers
  double * W = NULL;
  if(nlhs == 0)
  {
    return;
  }
  prepare_lhs(nlhs,plhs,no,W);

  double start_sec = igl::get_seconds();
  if(params.hierarchical && dim == 3)
  {
    // Prepare eigen versions of input
    MatrixXd MV(n,dim);
    copy(V,V+n*dim,MV.data());
    MatrixXi MF(m,dim);
    copy(F,F+m*dim,MF.data());
    // Initialize hierarchy
    AABB<Vector3d> hier(MV,MF);
    //hier.split_method = CENTER_ON_LONGEST_AXIS;
    hier.split_method = MEDIAN_ON_LONGEST_AXIS;
    // Build hierarchy
    hier.grow();
    // Be sure correct method is chosen
    BoundingTree<Vector3d>::method = EXACT_WINDING_NUMBER_METHOD;
    // loop over origins
#   pragma omp parallel for if (no>WINDING_NUMBER_OMP_MIN_VALUE)
    for(int o = 0;o<no;o++)
    {
      Vector3d p(O[0*no+o], O[1*no+o], O[2*no+o]);
      W[o] = hier.winding_number(p);
    }
  }else
  {
    switch(dim)
    {
      case 3:
        winding_number_3(V,n,F,m,O,no,W);
        break;
      case 2:
        winding_number_2(V,n,F,m,O,no,W);
        break;
    }
  }
  mexPrintf("Elapsed time is %g seconds\n",igl::get_seconds()-start_sec);

  // Restore the std stream buffer Important!
  delete[] F;
  std::cout.rdbuf(outbuf);
}

#endif
