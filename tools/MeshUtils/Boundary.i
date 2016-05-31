%include "std_shared_ptr.i"
%shared_ptr(PyMesh::Boundary);
%include "Mesh.i"
%{
#include "Boundary.h"
%}
%include "Boundary.h"
