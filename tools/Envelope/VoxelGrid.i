%include Mesh.i
%{
#include "VoxelGrid.h"
%}
%include "VoxelGrid.h"
%template(VoxelGrid2D) PyMesh::VoxelGrid<2>;
%template(VoxelGrid3D) PyMesh::VoxelGrid<3>;
