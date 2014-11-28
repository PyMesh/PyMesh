%include <std_string.i>
%include <std_shared_ptr.i>
%shared_ptr(ParameterManager)
%{
#include <Wires/Parameters/ParameterManager.h>
%}
%include "ParameterManager.h"
