#include "ZSparseMatrix.h"
#include <cassert>

void ZSparseMatrix::import_raw_csc(
        size_t num_rows, size_t num_cols,
        int* inner_idx_array, size_t inner_size,
        int* outer_idx_array, size_t outer_size,
        Float* value_array, size_t value_size) {
    assert(outer_size > 1);
    assert(inner_size == value_size);
    assert(num_cols == outer_size - 1);

    VectorI nnz(outer_size - 1);
    for (size_t i=1; i<outer_size; i++) {
        nnz[i-1] = outer_idx_array[i] - outer_idx_array[i-1];
    }

    resize(num_rows, num_cols);
    reserve(nnz);

    size_t col,row,val;
    for (size_t i=0; i<outer_size-1; i++) {
        col = i;
        const size_t start = outer_idx_array[i];
        const size_t end = outer_idx_array[i+1];
        for (size_t j=start; j<end; j++) {
            row = inner_idx_array[j];
            val = value_array[j];
            insert(row, col) = val;
        }
    }

    makeCompressed();
}
