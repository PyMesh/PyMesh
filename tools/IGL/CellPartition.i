%include "std_shared_ptr.i"
%shared_ptr(CellPartition);
%{
#include "CellPartition.h"
%}
%include "CellPartition.h"
