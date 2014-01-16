%{
#include "Assembler.h"
%}

%apply (double* IN_ARRAY1, int DIM1)      {(double* vector_field, int num_vector_field)}

%include "Assembler.h"
