%include "std_shared_ptr.i"
%shared_ptr(MinkowskiSum);
%{
#include "MinkowskiSum.h"
%}
%include "MinkowskiSum.h"
