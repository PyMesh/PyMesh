%include "std_shared_ptr.i"
%shared_ptr(CSGEngine);
%{
#include "CSGEngine.h"
%}
%include "std_string.i"
%include "CSGEngine.h"
