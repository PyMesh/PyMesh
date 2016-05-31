%include "std_shared_ptr.i"
%shared_ptr(PyMesh::BooleanEngine);
%{
#include "BooleanEngine.h"
%}
%include "std_string.i"
%include "BooleanEngine.h"
