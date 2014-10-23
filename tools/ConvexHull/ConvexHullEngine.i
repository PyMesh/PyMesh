%include "std_shared_ptr.i"
%shared_ptr(ConvexHullEngine);
%{
#include <ConvexHullEngine.h>
%}
%include "std_string.i"
%include "ConvexHullEngine.h"
