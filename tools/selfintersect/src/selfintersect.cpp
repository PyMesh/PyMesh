#include "selfintersect.h"
#include "SelfIntersectMesh.h"
#include <igl/C_STR.h>
#include <list>
#include <iostream>

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
    SelfintersectParam params;
    Eigen::MatrixXd vertices;
    Eigen::MatrixXi faces;

    SelfIntersectMesh SIM = SelfIntersectMesh(
            m_vertices, m_faces, params, vertices, faces);

    typedef std::list<int> FaceIdxList;
    const FaceIdxList& lIF = SIM.get_lIF();
    assert(lIF.size()%2 == 0);

    Eigen::MatrixXi& IF = m_intersecting_faces;
    IF.resize(lIF.size()/2,2);

    size_t count=0;
    for (FaceIdxList::const_iterator itr = lIF.begin();
            itr != lIF.end(); itr++) {
        IF(count/2, count%2) = *itr;
        count++;
    }


    m_vertices = vertices;
    m_faces = faces;

    size_t cur_num_faces = m_faces.rows();
    size_t num_new_faces = SIM.get_number_of_new_faces();
    assert(cur_num_faces >= num_new_faces);
    size_t base_count = cur_num_faces - num_new_faces;
    m_new_face_idx.resize(num_new_faces);
    for (size_t i=0; i<num_new_faces; i++) {
        m_new_face_idx[i] = base_count + i;
    }
}
