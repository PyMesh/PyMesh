%include "std_shared_ptr.i"
%shared_ptr(PyMesh::SelfIntersectionResolver);
%{
#include "SelfIntersectionResolver.h"
%}
%include "std_string.i"
%include "SelfIntersectionResolver.h"
