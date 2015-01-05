%include "std_shared_ptr.i"
%shared_ptr(TetrahedronizationEngine);
%{
#include "TetrahedronizationEngine.h"
%}
%include "std_string.i"
%include "TetrahedronizationEngine.h"
