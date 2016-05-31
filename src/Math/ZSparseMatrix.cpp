/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ZSparseMatrix.h"
#include <cassert>
#include <iostream>
#include <vector>

using namespace PyMesh;

void ZSparseMatrix::import_raw_csc(
        size_t num_rows, size_t num_cols,
        int* inner_idx_array, size_t inner_size,
        int* outer_idx_array, size_t outer_size,
        double* value_array, size_t value_size) {
    assert(outer_size > 1);
    assert(inner_size == value_size);
    assert(num_cols == outer_size - 1);

    VectorI nnz(outer_size - 1);
    for (size_t i=1; i<outer_size; i++) {
        nnz[i-1] = outer_idx_array[i] - outer_idx_array[i-1];
    }

    resize(num_rows, num_cols);
    reserve(value_size);

    size_t col,row;
    double val;
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
    assert(isCompressed());
    assert(outer_size == get_outer_size());
    assert(inner_size == get_inner_size());
    assert(value_size == get_value_size());
    for (size_t i=0; i<outer_size; i++) {
        assert(outer_idx_array[i] == outerIndexPtr()[i]);
    }
    for (size_t i=0; i<inner_size; i++) {
        assert(inner_idx_array[i] == innerIndexPtr()[i]);
        assert(value_array[i] == valuePtr()[i]);
    }
}

void ZSparseMatrix::import_raw_coo(
        size_t num_rows, size_t num_cols,
        int* row_indices, size_t num_row_indices,
        int* col_indices, size_t num_col_indices,
        double* value_array, size_t value_size) {
    assert(num_row_indices == num_col_indices);
    assert(num_row_indices == value_size);
    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;
    for (size_t i=0; i<value_size; i++) {
        entries.push_back(T(row_indices[i], col_indices[i], value_array[i]));
    }

    resize(num_rows, num_cols);
    setFromTriplets(entries.begin(), entries.end());
    makeCompressed();
    assert(isCompressed());
}
