%{
#include "MeshFactory.h"
%}
%include "std_string.i"
%newobject MeshFactory::create();
%include "MeshFactory.h"
