/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <DMAT.h>

class DMATTest : public TestBase {
};

TEST_F(DMATTest, MatrixF) {
    MatrixF A(3, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9;
    std::string tmp_name = "tmp_A.dmat";
    DMAT::save_matrix(tmp_name, A);

    MatrixF B = DMAT::load_matrix<MatrixF>(tmp_name);
    ASSERT_MATRIX_EQ(A, B);
}

TEST_F(DMATTest, MatrixFr) {
    MatrixFr A(3, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9;
    std::string tmp_name = "tmp_A.dmat";
    DMAT::save_matrix(tmp_name, A);

    MatrixFr B = DMAT::load_matrix<MatrixFr>(tmp_name);
    ASSERT_MATRIX_EQ(A, B);
}

TEST_F(DMATTest, MixedOrder) {
    MatrixF A(3, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9;
    std::string tmp_name = "tmp_A.dmat";
    DMAT::save_matrix(tmp_name, A);

    MatrixFr B = DMAT::load_matrix<MatrixFr>(tmp_name);
    ASSERT_MATRIX_EQ(A, B);
}

TEST_F(DMATTest, MatrixI) {
    MatrixI A(3, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9;
    std::string tmp_name = "tmp_A.dmat";
    DMAT::save_matrix(tmp_name, A);

    MatrixI B = DMAT::load_matrix<MatrixI>(tmp_name);
    ASSERT_MATRIX_EQ(A, B);
}

TEST_F(DMATTest, VectorF) {
    VectorF A(9);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9;
    std::string tmp_name = "tmp_A.dmat";
    DMAT::save_matrix(tmp_name, A);

    VectorF B = DMAT::load_matrix<VectorF>(tmp_name);
    ASSERT_MATRIX_EQ(A, B);
}

TEST_F(DMATTest, ascii) {
    VectorF A(9);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9;
    std::string tmp_name = "tmp_A.dmat";
    DMAT::save_matrix(tmp_name, A, true);

    VectorF B = DMAT::load_matrix<VectorF>(tmp_name);
    ASSERT_MATRIX_EQ(A, B);
}
