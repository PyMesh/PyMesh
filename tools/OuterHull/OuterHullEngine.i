%include "std_shared_ptr.i"
%shared_ptr(PyMesh::OuterHullEngine);
%{
#include "OuterHullEngine.h"
%}
%include "std_string.i"
%include "OuterHullEngine.h"
