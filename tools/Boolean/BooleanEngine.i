%include "std_shared_ptr.i"
%shared_ptr(BooleanEngine);
%{
#include "BooleanEngine.h"
%}
%include "std_string.i"
%include "BooleanEngine.h"
