%include "std_shared_ptr.i"
%shared_ptr(PyMesh::MinkowskiSum);
%{
#include "MinkowskiSum.h"
%}
%include "MinkowskiSum.h"
