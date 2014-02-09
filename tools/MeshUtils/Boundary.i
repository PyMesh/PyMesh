#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
%include "std_shared_ptr.i"
%shared_ptr(Boundary);
%include "Mesh.i"
%{
#include "Boundary.h"
%}
%include "Boundary.h"
