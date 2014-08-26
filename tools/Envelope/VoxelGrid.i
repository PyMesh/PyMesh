%{
#include "VoxelGrid.h"
%}
%include "VoxelGrid.h"
%template(VoxelGrid2D) VoxelGrid<2>;
%template(VoxelGrid3D) VoxelGrid<3>;
