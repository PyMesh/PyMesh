%include "std_shared_ptr.i"
%shared_ptr(Subdivision);
%{
#include "Subdivision.h"
%}
%include "Subdivision.h"
