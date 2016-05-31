%{
#include "FEAssembler.h"
%}
%pythonbegin %{
import numpy as np
import scipy.sparse
%}
%pythoncode %{
def ZSparseMatrix2Scipy(matrix):
    """
    Convert a C++ type ZSparseMatrix to python type scipy.sparse.csc_matrix.
    """
    data = np.ndarray(matrix.get_value_size(), dtype=float);
    outer_idx = np.ndarray(matrix.get_outer_size(), dtype=np.int32);
    inner_idx = np.ndarray(matrix.get_inner_size(), dtype=np.int32);

    matrix.get_values(data);
    matrix.get_outer_indices(outer_idx);
    matrix.get_inner_indices(inner_idx);

    return scipy.sparse.csc_matrix((data, inner_idx, outer_idx),
        shape = (matrix.num_rows(), matrix.num_cols()),
        dtype = float);
%}

// Modified the result type of assemble method to return a scipy matrix.
%pythonappend FEAssembler::assemble(const std::string&) %{
    val = ZSparseMatrix2Scipy(val);
%}
%include FEAssembler.h
