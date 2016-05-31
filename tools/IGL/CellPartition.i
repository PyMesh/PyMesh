%include "std_shared_ptr.i"
%shared_ptr(PyMesh::CellPartition);
%{
#include "CellPartition.h"
%}
%include "CellPartition.h"
