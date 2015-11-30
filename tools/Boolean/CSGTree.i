%include "std_shared_ptr.i"
%shared_ptr(CSGTree);
%{
#include "CSGTree.h"
%}
%include "std_string.i"
%include "CSGTree.h"
