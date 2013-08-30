#include "selfintersect.h"
#include "SelfIntersectMesh.h"
#include <igl/C_STR.h>
#include <list>

#ifdef MEX
#  include <mex.h>
#  undef assert
#  define assert( isOK ) ( (isOK) ? (void)0 : (void) mexErrMsgTxt(C_STR(__FILE__<<":"<<__LINE__<<": failed assertion `"<<#isOK<<"'"<<std::endl) ) )
#endif

void selfintersect(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  const SelfintersectParam & params,
  Eigen::MatrixXd & VV,
  Eigen::MatrixXi & FF,
  Eigen::MatrixXi & IF)
{
  using namespace std;
  SelfIntersectMesh SIM = SelfIntersectMesh(V,F,params,VV,FF);
  // Convert lIF to Eigen matrix
  assert(SIM.get_lIF().size()%2 == 0);
  IF.resize(SIM.get_lIF().size()/2,2);
  {
    int i=0;
    for(
      list<int>::const_iterator ifit = SIM.get_lIF().begin();
      ifit!=SIM.get_lIF().end();
      )
    {
      IF(i,0) = (*ifit);
      ifit++; 
      IF(i,1) = (*ifit);
      ifit++;
      i++;
    }
  }
}

void selfintersect_python(
        const Eigen::MatrixXd & V,
        const Eigen::MatrixXi & F,
        Eigen::MatrixXd & VV,
        Eigen::MatrixXi & FF) {
    SelfintersectParam params;
    Eigen::MatrixXi IF;

    selfintersect(V, F, params, VV, FF, IF);
}

void SelfIntersectionResolver::resolve() {
}
