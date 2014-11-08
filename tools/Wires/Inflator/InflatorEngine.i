%include <std_string.i>
%include <std_shared_ptr.i>
%shared_ptr(InflatorEngine)
%{
#include <Wires/Inflator/InflatorEngine.h>
%}
%include "InflatorEngine.h"
