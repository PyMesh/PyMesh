%include <std_string.i>
%include <std_shared_ptr.i>
%shared_ptr(WireNetwork)
%{
#include <Wires/WireNetwork/WireNetwork.h>
%}
%include "WireNetwork.h"
