/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Misc/MatrixIO.h>

TEST(MatrixIOTest, col_major) {
    MatrixF M(2, 3);
    M << 1, 2, 3, 4, 5, 6;
    MatrixIO::save_matrix("m.npy", M);

    MatrixF R = MatrixIO::load_matrix<MatrixF>("m.npy");

    ASSERT_FLOAT_EQ(0.0, (M - R).norm());
}

TEST(MatrixIOTest, row_major) {
    MatrixFr M(2, 3);
    M << -1, -2, -3, -4, -5, -6;
    MatrixIO::save_matrix("m.npy", M);

    MatrixFr R = MatrixIO::load_matrix<MatrixFr>("m.npy");

    ASSERT_FLOAT_EQ(0.0, (M - R).norm());
}

TEST(MatrixIOTest, vector) {
    VectorF v(4);
    v << 1, 2, 3, 4;
    MatrixIO::save_matrix("m.npy", v);

    VectorF r = MatrixIO::load_matrix<VectorF>("m.npy");
    ASSERT_FLOAT_EQ(0.0, (v-r).norm());
}

TEST(MatrixIOTest, int_matrix) {
    MatrixIr M(3, 2);
    M << -1, -2, -3, -4, -5, -6;
    MatrixIO::save_matrix("m.npy", M);

    MatrixIr R = MatrixIO::load_matrix<MatrixIr>("m.npy");

    ASSERT_FLOAT_EQ(0.0, (M - R).norm());
}

TEST(MatrixIOTest, int_vector) {
    VectorI v(3);
    v << -1, 2, 3;
    MatrixIO::save_matrix("m.npy", v);

    VectorI r = MatrixIO::load_matrix<VectorI>("m.npy");

    ASSERT_FLOAT_EQ(0.0, (v - r).norm());
}
