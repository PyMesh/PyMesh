%{
#include "HashGrid.h"
%}
#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
%include <std_shared_ptr.i>
%shared_ptr(Zhou::HashGrid)
%include "HashGrid.h"
