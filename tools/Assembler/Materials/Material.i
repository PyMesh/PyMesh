#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
%include "std_shared_ptr.i"
%shared_ptr(Material);
%{
#include "Materials/Material.h"
%}
%include "Materials/Material.h"
