%include "std_shared_ptr.i"
%shared_ptr(PyMesh::CSGTree);
%{
#include "CSGTree.h"
%}
%include "std_string.i"
%include "CSGTree.h"
