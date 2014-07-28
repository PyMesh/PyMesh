%{
#include "Math/ZSparseMatrix.h"
%}
%apply (int* INPLACE_ARRAY1, int DIM1) {(int* np_idx_array, size_t idx_size)}
%apply (double* INPLACE_ARRAY1, int DIM1) {(double* np_value_array, size_t num_values)}
%apply (double** ARGOUTVIEW_ARRAY1, int* DIM1) {(double** np_value_array, int* num_values)}
%apply (int* IN_ARRAY1, int DIM1) {(int* inner_idx_array, size_t inner_size)}
%apply (int* IN_ARRAY1, int DIM1) {(int* outer_idx_array, size_t outer_size)}
%apply (double* IN_ARRAY1, int DIM1) {(Float* value_array, size_t value_size)}
%include Math/ZSparseMatrix.h
