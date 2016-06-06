%{
#include <CGAL/AABBTree.h>
%}
%apply PyMesh::VectorF& ARGOUT { PyMesh::VectorF& squared_dists }
%apply PyMesh::VectorI& ARGOUT { PyMesh::VectorI& closest_face_indices }
%apply PyMesh::MatrixFr& ARGOUT { PyMesh::MatrixFr& closest_points }
%include "AABBTree.h"
