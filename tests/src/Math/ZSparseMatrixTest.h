/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Math/ZSparseMatrix.h>

class ZSparseMatrixTest : public ::testing::Test {
    protected:
        typedef Eigen::Triplet<Float> T;

        virtual void SetUp() {
            const size_t size = 16;
            m_matrix.resize(size, size);

            std::vector<T> entries;
            for (size_t i=0; i<size; i++) {
                for (size_t j=0; j<size; j++) {
                    entries.push_back(T(i, j, Float(i+j+1)));
                }
            }
            m_matrix.setFromTriplets(entries.begin(), entries.end());

            // Check matrix initialization
            ASSERT_EQ(size, m_matrix.rows());
            ASSERT_EQ(size, m_matrix.cols());
            for (size_t i=0; i<size; i++) {
                for (size_t j=0; j<size; j++) {
                    ASSERT_FLOAT_EQ(Float(i+j+1), m_matrix.coeff(i, j));
                }
            }
        }

        void ASSERT_SPARSE_MATRIX_EQ(const ZSparseMatrix& m1, const ZSparseMatrix& m2) {
            ASSERT_EQ(m1.rows(), m2.rows());
            ASSERT_EQ(m1.cols(), m2.cols());
            ZSparseMatrix diff(m1 - m2);
            ASSERT_FLOAT_EQ(0.0, diff.norm());
        }

    protected:
        ZSparseMatrix m_matrix;
};

TEST_F(ZSparseMatrixTest, Copy) {
    ZSparseMatrix m2 = m_matrix;
    ASSERT_SPARSE_MATRIX_EQ(m_matrix, m2);
}

TEST_F(ZSparseMatrixTest, ImportCSC) {
    const size_t inner_size = m_matrix.get_inner_size();
    const size_t outer_size = m_matrix.get_outer_size();
    const size_t value_size = m_matrix.get_value_size();

    int* inner_idx = new int[inner_size];
    int* outer_idx = new int[outer_size];
    Float* values = new Float[value_size];

    m_matrix.get_inner_indices(inner_idx, inner_size);
    m_matrix.get_outer_indices(outer_idx, outer_size);
    m_matrix.get_values(values, value_size);

    ZSparseMatrix m2;
    m2.import_raw_csc(m_matrix.rows(), m_matrix.cols(),
            inner_idx, inner_size,
            outer_idx, outer_size,
            values, value_size);

    ASSERT_SPARSE_MATRIX_EQ(m_matrix, m2);

    delete [] inner_idx;
    delete [] outer_idx;
    delete [] values;
}
