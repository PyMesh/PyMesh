%{
#include "tetgen_wrapper.h"
%}

%include "std_string.i"
//%newobject TetgenWrapper::tetrahedronize_mesh;
%newobject TetgenWrapper::tetrahedronize_raw;
%include "tetgen_wrapper.h"
