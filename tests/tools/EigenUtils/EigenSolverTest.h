/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <set>

#include <Core/Exception.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

#include <EigenUtils/EigenSolver.h>

class EigenUtilsTest : public ::testing::Test {
    protected:
        void ASSERT_MATRIX_EQ(const MatrixF& M1, const MatrixF& M2) {
            ASSERT_EQ(M1.rows(), M2.rows());
            ASSERT_EQ(M1.cols(), M2.cols());
            ASSERT_TRUE(M1 == M2);
        }

    protected:
        EigenSolver m_solver;
};

TEST_F(EigenUtilsTest, 2x2) {
    MatrixF M(2,2);
    M << 1, 0,
         0, 2;
    m_solver.compute(M);
    VectorF e_values = m_solver.get_eigen_values();
    MatrixF e_vectors = m_solver.get_eigen_vectors();
    ASSERT_FLOAT_EQ(2.0, e_values.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, e_values.minCoeff());
    ASSERT_MATRIX_EQ(MatrixF::Identity(2, 2), e_vectors);
}

TEST_F(EigenUtilsTest, 3x3) {
    MatrixF M(3,3);
    M << 1, 0, 0,
         0, 2, 0,
         0, 0, 3;
    m_solver.compute(M);
    VectorF e_values = m_solver.get_eigen_values();
    MatrixF e_vectors = m_solver.get_eigen_vectors();
    ASSERT_FLOAT_EQ(3.0, e_values.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, e_values.minCoeff());
    ASSERT_MATRIX_EQ(MatrixF::Identity(3, 3), e_vectors);
}

TEST_F(EigenUtilsTest, 4x4) {
    MatrixF M(4,4);
    M << 1, 0, 0, 0,
         0, 2, 0, 0,
         0, 0, 3, 0,
         0, 0, 0, 4;
    m_solver.compute(M);
    VectorF e_values = m_solver.get_eigen_values();
    MatrixF e_vectors = m_solver.get_eigen_vectors();
    ASSERT_FLOAT_EQ(4.0, e_values.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, e_values.minCoeff());
    ASSERT_MATRIX_EQ(MatrixF::Identity(4, 4), e_vectors);
}

TEST_F(EigenUtilsTest, batch_2x2) {
    const size_t n = 2;
    VectorF Ms = VectorF::Zero(n * 3);
    Ms[0] = 1.0;
    Ms[1] = 1.0;
    Ms[3] = 1.0;
    Ms[4] = 1.0;
    m_solver.compute_batch_symmetric(2, Ms);
    VectorF e_values = m_solver.get_eigen_values();
    MatrixF e_vectors = m_solver.get_eigen_vectors();
    ASSERT_EQ(4, e_values.size());
    ASSERT_FLOAT_EQ(1.0, e_values.minCoeff());
    ASSERT_FLOAT_EQ(1.0, e_values.maxCoeff());

    MatrixF true_eigen_vectors(4, 2);
    true_eigen_vectors << 1.0, 0.0,
                          0.0, 1.0,
                          1.0, 0.0,
                          0.0, 1.0;
    ASSERT_MATRIX_EQ(true_eigen_vectors, e_vectors);
}

TEST_F(EigenUtilsTest, batch_3x3) {
    const size_t n = 2;
    VectorF Ms = VectorF::Zero(n * 6);
    Ms[0] = 1.0;
    Ms[1] = 1.0;
    Ms[2] = 1.0;
    Ms[6] = 2.0;
    Ms[7] = 2.0;
    Ms[8] = 2.0;
    m_solver.compute_batch_symmetric(3, Ms);
    VectorF e_values = m_solver.get_eigen_values();
    MatrixF e_vectors = m_solver.get_eigen_vectors();
    ASSERT_EQ(n*3, e_values.size());
    ASSERT_FLOAT_EQ(1.0, e_values.minCoeff());
    ASSERT_FLOAT_EQ(2.0, e_values.maxCoeff());

    MatrixF true_eigen_vectors(6, 3);
    true_eigen_vectors << 1.0, 0.0, 0.0,
                          0.0, 1.0, 0.0,
                          0.0, 0.0, 1.0,
                          1.0, 0.0, 0.0,
                          0.0, 1.0, 0.0,
                          0.0, 0.0, 1.0;
    ASSERT_MATRIX_EQ(true_eigen_vectors, e_vectors);
}

