%{
#include "ZSparseMatrix.h"
#include "ZMatrix.h"
%}
%include "std_vector.i"

// Translate int and double vector into python lists.
namespace std {
    %template(IntVector) vector<int>;
    %template(DoubleVector) vector<double>;
}

%include "carrays.i"
%array_class(double, doubleArray);
%array_class(int, intArray);

%apply (int* INPLACE_ARRAY1, int DIM1) {(int* np_idx_array, size_t idx_size)}
%apply (double* INPLACE_ARRAY1, int DIM1) {(double* np_value_array, size_t num_values)}
%apply (double** ARGOUTVIEW_ARRAY1, int* DIM1) {(double** np_value_array, int* num_values)}

%include "ZSparseMatrix.h"
//%include "ZMatrix.h"

