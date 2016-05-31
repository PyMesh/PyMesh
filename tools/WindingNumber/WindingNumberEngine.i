%include "std_shared_ptr.i"
%shared_ptr(PyMesh::WindingNumberEngine);
%{
#include "WindingNumberEngine.h"
%}
%include "std_string.i"
%include "WindingNumberEngine.h"
