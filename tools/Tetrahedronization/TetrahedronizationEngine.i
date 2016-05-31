%include "std_shared_ptr.i"
%shared_ptr(PyMesh::TetrahedronizationEngine);
%{
#include "TetrahedronizationEngine.h"
%}
%include "std_string.i"
%include "TetrahedronizationEngine.h"
