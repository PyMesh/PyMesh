/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Eigen/Sparse>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class ZSparseMatrix : public Eigen::SparseMatrix<Float, Eigen::ColMajor, int> {
    public:
        typedef Eigen::SparseMatrix<Float, Eigen::ColMajor, int> ParentType;
        ZSparseMatrix() {}
        ZSparseMatrix(size_t rows, size_t cols) : ParentType(rows, cols) {}
        ZSparseMatrix(const ParentType& other) : ParentType(other) {}
        virtual ~ZSparseMatrix() { }
        ZSparseMatrix& operator=(const ParentType& other) {
            ParentType::operator=(other);
            return *this;
        }

    public:
        size_t num_rows() const { return rows(); }
        size_t num_cols() const { return cols(); }
        size_t get_inner_size() const { return nonZeros(); }
        size_t get_outer_size() const { return cols() + 1; }
        size_t get_value_size() const { return nonZeros(); }

        void get_inner_indices(int* np_idx_array, size_t idx_size) {
            assert(idx_size <= get_inner_size());
            int* inner = innerIndexPtr();
            for (size_t i=0; i<idx_size; i++) {
                np_idx_array[i] = inner[i];
            }
        }

        void get_outer_indices(int* np_idx_array, size_t idx_size) {
            assert(idx_size <= get_outer_size());
            int* outer = outerIndexPtr();
            for (size_t i=0; i<idx_size; i++) {
                np_idx_array[i] = outer[i];
            }
        }

        void get_values(double* np_value_array, size_t num_values) {
            assert(num_values <= get_value_size());
            double* values = valuePtr();
            for (size_t i=0; i<num_values; i++) {
                np_value_array[i] = values[i];
            }
        }

        /**
         * Initialize matrix from raw data in compressed sparse column format.
         */
        void import_raw_csc(size_t num_rows, size_t num_cols,
                int* inner_idx_array, size_t inner_size,
                int* outer_idx_array, size_t outer_size,
                double* value_array, size_t value_size);

        void import_raw_coo(size_t num_rows, size_t num_cols,
                int* row_indices, size_t num_row_indices,
                int* col_indices, size_t num_col_indices,
                double* value_array, size_t value_size);
};

}
