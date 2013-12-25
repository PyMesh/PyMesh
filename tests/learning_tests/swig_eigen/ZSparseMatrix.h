#pragma once

#include <Eigen/Sparse>

#include <Core/EigenTypedef.h>

class ZSparseMatrix : public Eigen::SparseMatrix<Float, Eigen::ColMajor> {
    public:
        typedef Eigen::SparseMatrix<Float> ParentType;
        ZSparseMatrix() {}
        ZSparseMatrix(size_t rows, size_t cols) : ParentType(rows, cols) {}

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
};
