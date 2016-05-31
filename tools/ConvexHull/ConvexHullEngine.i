%include "std_shared_ptr.i"
%shared_ptr(PyMesh::ConvexHullEngine);
%{
#include <ConvexHullEngine.h>
%}
%include "std_string.i"
%include "ConvexHullEngine.h"
