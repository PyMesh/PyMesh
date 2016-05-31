%include "std_shared_ptr.i"
%shared_ptr(PyMesh::Subdivision);
%{
#include "Subdivision.h"
%}
%include "Subdivision.h"
