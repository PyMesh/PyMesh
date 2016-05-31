%include <std_shared_ptr.i>
%shared_ptr(PyMesh::WireProfile)
%{
#include <Wires/Inflator/WireProfile.h>
%}
%include "WireProfile.h"
