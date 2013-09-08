#include "tesselate_boundary.h"
#include "exterior_edges.h"
#include <igl/list_to_matrix.h>

void tesselate_boundary(
  const Eigen::MatrixXi & F,
  Eigen::MatrixXi & cap)
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;

  MatrixXi boundary;
  exterior_edges(F,boundary);
  // Handle lame base case
  if(boundary.size() == 0)
  {
    cap.resize(0,F.cols());
    return;
  }
  // "Triangulate" aka "close" the boundary trivially with facets
  // Note: in 2D we need to know if boundary endpoints are incoming or
  // outgoing. Thus this will not work.
  assert(F.cols() == 3);
  // Arbitrary starting vertex
  int s = boundary(int(((double)rand() / RAND_MAX)*boundary.rows()),0);
  vector<vector<int> >  lcap;
  for(int i = 0;i<boundary.rows();i++)
  {
    // Skip edges incident on s (they would be zero-area)
    if(boundary(i,0) == s || boundary(i,1) == s)
    {
      continue;
    }
    vector<int> e(3);
    e[0] = s;
    e[1] = boundary(i,0);
    e[2] = boundary(i,1);
    lcap.push_back(e);
  }
  list_to_matrix(lcap,cap);
}

Eigen::MatrixXi tesselate_boundary( const Eigen::MatrixXi & F)
{
  Eigen::MatrixXi cap;
  tesselate_boundary(F,cap);
  return cap;
}
