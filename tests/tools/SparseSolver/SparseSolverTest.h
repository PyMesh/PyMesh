/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <vector>
#include <random>

#include <Eigen/Sparse>

#include <SparseSolver/SparseSolver.h>
#include <SparseSolver/SparseSolverFactory.h>

#include <TestBase.h>

class SparseSolverTest : public TestBase {
    protected:
        typedef ZSparseMatrix SMat;
        typedef Eigen::Triplet<Float> T;
        typedef SparseSolver::Ptr SolverPtr;

        void init_diagonal_matrix() {
            const size_t size = 1024 * 8;
            std::vector<T> entries;
            for (size_t i=0; i<size; i++) {
                entries.push_back(T(i, i, 1.0));
            }

            m_matrix.resize(size, size);
            m_matrix.setFromTriplets(entries.begin(), entries.end());

            // Check matrix is correctly built.
            ASSERT_EQ(size, m_matrix.rows());
            ASSERT_EQ(size, m_matrix.cols());
            for (size_t i=0; i<size; i++) {
                ASSERT_FLOAT_EQ(1.0, m_matrix.coeff(i, i));
            }
        }

        void init_dense_matrix() {
            std::default_random_engine generator;
            std::uniform_real_distribution<Float> distribution(0.0,1.0);

            const size_t size = 1024;
            std::vector<T> entries;
            for (size_t i=0; i<size; i++) {
                for (size_t j=i; j<size; j++) {
                    if (i == j) {
                        entries.push_back(T(i, j, 20.0+distribution(generator)));
                    } else {
                        Float val = distribution(generator);
                        entries.push_back(T(i, j, val));
                        entries.push_back(T(j, i, val));
                    }
                }
            }

            m_matrix.resize(size, size);
            m_matrix.setFromTriplets(entries.begin(), entries.end());

            // Check matrix is correctly built.
            ASSERT_EQ(size, m_matrix.rows());
            ASSERT_EQ(size, m_matrix.cols());
            for (auto item : entries) {
                ASSERT_FLOAT_EQ(item.value(), m_matrix.coeff(item.row(), item.col()));
                //std::cout << item.row() << " " << item.col() << " " << item.value() << std::endl;
            }
        }

        SolverPtr create(const std::string& type) {
            return SparseSolverFactory(type).create();
        }

        void ASSERT_VECTOR_EQ(const VectorF& v1, const VectorF& v2) {
            ASSERT_EQ(v1.size(), v2.size());
            ASSERT_NEAR(0.0, (v1-v2).norm(), 1e-6);
        }

        void solve_diagonal_system(const std::string& type) {
            init_diagonal_matrix();
            SolverPtr solver = create(type);
            VectorF rhs = VectorF::Ones(m_matrix.rows());

            solver->analyze_pattern(m_matrix);
            solver->factorize(m_matrix);
            VectorF sol = solver->solve(rhs);

            ASSERT_VECTOR_EQ(rhs, m_matrix * sol);
        }

        void solve_dense_system(const std::string& type) {
            init_dense_matrix();
            SolverPtr solver = create(type);
            VectorF rhs = VectorF::Ones(m_matrix.rows());

            solver->analyze_pattern(m_matrix);
            solver->factorize(m_matrix);
            VectorF sol = solver->solve(rhs);

            ASSERT_VECTOR_EQ(rhs, m_matrix * sol);
        }

    protected:
        SMat m_matrix;
};

/**
 * TODO:
 * Eigen::SimplicialLLT gives a segmentation fault which I haven't got a chance
 * to track it down.  This test is disabled for now.
 */
TEST_F(SparseSolverTest, DISABLED_LLT) {
    solve_diagonal_system("LLT");
    solve_dense_system("LLT");
}

TEST_F(SparseSolverTest, DISABLED_LDLT) {
    solve_diagonal_system("LDLT");
    solve_dense_system("LDLT");
}

TEST_F(SparseSolverTest, DISABLED_CG) {
    solve_diagonal_system("CG");
    solve_dense_system("CG");
}

TEST_F(SparseSolverTest, DISABLED_SparseLU) {
    solve_diagonal_system("SparseLU");
    solve_dense_system("SparseLU");
}

TEST_F(SparseSolverTest, DISABLED_UmfPackLU) {
    solve_diagonal_system("UmfPackLU");
    solve_dense_system("UmfPackLU");
}

TEST_F(SparseSolverTest, DISABLED_UmfPack) {
    solve_diagonal_system("UmfPack");
    solve_dense_system("UmfPack");
}

TEST_F(SparseSolverTest, DISABLED_Cholmod) {
    solve_diagonal_system("Cholmod");
    solve_dense_system("Cholmod");
}
