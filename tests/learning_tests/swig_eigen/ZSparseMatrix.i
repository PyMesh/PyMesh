%{
#include "ZSparseMatrix.h"
%}
%apply (int* INPLACE_ARRAY1, int DIM1) {(int* np_idx_array, size_t idx_size)}
%apply (double* INPLACE_ARRAY1, int DIM1) {(double* np_value_array, size_t num_values)}
%apply (double** ARGOUTVIEW_ARRAY1, int* DIM1) {(double** np_value_array, int* num_values)}
%include ZSparseMatrix.h
