/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>

#include <TestBase.h>

class MatrixUtilsTest : public TestBase {
    protected:
        template<typename Derived>
        void fill_with_index(Eigen::MatrixBase<Derived>& M) {
            const size_t num_rows = M.rows();
            const size_t num_cols = M.cols();
            const size_t num_entires = num_rows * num_cols;

            Derived& M2 = static_cast<Derived&>(M);
            for (size_t i=0; i<num_entires; i++) {
                M2.data()[i] = (typename Derived::Scalar)(i);
            }
        }
};

TEST_F(MatrixUtilsTest, index_fill) {
    VectorI v(4);
    fill_with_index(v);

    for (size_t i=0; i<4; i++) {
        ASSERT_EQ(i, v[i]);
    }
}

TEST_F(MatrixUtilsTest, vstack) {
    MatrixFr m(2, 2);
    fill_with_index(m);

    std::vector<MatrixFr> matrices;
    matrices.push_back(m);
    matrices.push_back(m);

    MatrixFr stacked_m = MatrixUtils::vstack(matrices);

    ASSERT_EQ(4, stacked_m.rows());
    ASSERT_EQ(2, stacked_m.cols());
    ASSERT_MATRIX_EQ(m, stacked_m.block(0, 0, 2, 2));
    ASSERT_MATRIX_EQ(m, stacked_m.block(2, 0, 2, 2));
}

TEST_F(MatrixUtilsTest, rowstack) {
    Vector2F v(1, 2);
    fill_with_index(v);

    std::vector<VectorF> rows;
    rows.push_back(v);
    rows.push_back(v);
    rows.push_back(v);

    MatrixFr stacked_m = MatrixUtils::rowstack(rows);
    ASSERT_EQ(3, stacked_m.rows());
    ASSERT_EQ(2, stacked_m.cols());
    ASSERT_FLOAT_EQ(0.0, (stacked_m.rowwise() - v.transpose()).squaredNorm());
}

TEST_F(MatrixUtilsTest, colstack) {
    Vector2F v(1, 2);
    fill_with_index(v);

    std::vector<VectorF> rows;
    rows.push_back(v);
    rows.push_back(v);
    rows.push_back(v);

    MatrixF stacked_m = MatrixUtils::colstack(rows);
    ASSERT_EQ(2, stacked_m.rows());
    ASSERT_EQ(3, stacked_m.cols());
    ASSERT_FLOAT_EQ(0.0, (stacked_m.colwise() - v).squaredNorm());
}

TEST_F(MatrixUtilsTest, reshape_int) {
    VectorI v(4);
    fill_with_index(v);
    MatrixIr m = MatrixUtils::reshape<MatrixIr>(v, 2, 2);

    MatrixIr target_m(2, 2);
    fill_with_index(target_m);
    ASSERT_MATRIX_EQ(target_m, m);
}

TEST_F(MatrixUtilsTest, reshape_float) {
    VectorF v(4);
    fill_with_index(v);
    MatrixFr m = MatrixUtils::reshape<MatrixFr>(v, 2, 2);

    MatrixFr target_m(2, 2);
    fill_with_index(target_m);
    ASSERT_MATRIX_EQ(target_m, m);
}

TEST_F(MatrixUtilsTest, reshape_transpose) {
    // m1 is row major
    MatrixFr m1(2, 4);
    fill_with_index(m1);

    // m2 is col major, or m2 is actually m1.transpose()
    MatrixF m2 = MatrixUtils::reshape<MatrixF>(m1, 4, 2);

    MatrixF m1t = m1.transpose();
    ASSERT_MATRIX_EQ(m1t, m2);
}

TEST_F(MatrixUtilsTest, reshape_to_1D) {
    MatrixFr m(2, 2);
    fill_with_index(m);

    VectorF v = MatrixUtils::reshape<VectorF>(m, 4, 1);

    VectorF target(4);
    fill_with_index(target);
    ASSERT_MATRIX_EQ(target, v);
}

TEST_F(MatrixUtilsTest, reshape_with_copy) {
    MatrixFr m1(2, 2);
    fill_with_index(m1);

    MatrixFr m2 = MatrixUtils::reshape<MatrixFr>(m1, 4, 1);
    ASSERT_FLOAT_EQ(m1(0, 0), m2(0, 0));

    m1(0, 0) = -1;
    ASSERT_FLOAT_EQ(-1.0, m1(0, 0));
    ASSERT_FLOAT_EQ( 0.0, m2(0, 0));
}

TEST_F(MatrixUtilsTest, flatten_vectori) {
    VectorI v(4);
    fill_with_index(v);

    VectorI v2 = MatrixUtils::flatten<VectorI>(v);

    ASSERT_MATRIX_EQ(v, v2);
}

TEST_F(MatrixUtilsTest, flatten_vectorf) {
    VectorF v(4);
    fill_with_index(v);

    VectorF v2 = MatrixUtils::flatten<VectorF>(v);

    ASSERT_MATRIX_EQ(v, v2);
}

TEST_F(MatrixUtilsTest, flatten_matrixi) {
    MatrixIr m(2, 2);
    fill_with_index(m);

    VectorI v = MatrixUtils::flatten<VectorI>(m);
    VectorI v2 = MatrixUtils::reshape<VectorI>(m, 4, 1);

    ASSERT_MATRIX_EQ(v, v2);
}

TEST_F(MatrixUtilsTest, flatten_matrixf) {
    MatrixFr m(2, 2);
    fill_with_index(m);

    VectorF v = MatrixUtils::flatten<VectorF>(m);
    VectorF v2 = MatrixUtils::reshape<VectorF>(m, 4, 1);

    ASSERT_MATRIX_EQ(v, v2);
}

