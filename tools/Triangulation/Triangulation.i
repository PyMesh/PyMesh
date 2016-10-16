%include "std_shared_ptr.i"
%shared_ptr(PyMesh::Triangulation);
%{
#include "Triangulation.h"
%}
%include "Triangulation.h"
