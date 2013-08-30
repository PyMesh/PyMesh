/* Setup the interface between numpy and C array
   This file should only be included once in the entire module
*/

%{
#define SWIG_FILE_WITH_INIT
%}
%include "numpy.i"
%init %{
import_array();
%}

