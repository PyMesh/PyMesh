#pragma once
#include <string>
#include <vector>

#include <Eigen/Sparse>

#include <EigenUtils/SparseSolver.h>
#include <EigenUtils/SparseSolverFactory.h>

class SparseSolverTest : public ::testing::Test {
    protected:
        typedef ZSparseMatrix SMat;
        typedef Eigen::Triplet<Float> T;
        typedef SparseSolver::Ptr SolverPtr;

        void SetUp() {
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

        SolverPtr create(const std::string& type) {
            return SparseSolverFactory(type).create();
        }

        void ASSERT_VECTOR_EQ(const VectorF& v1, const VectorF& v2) {
            ASSERT_EQ(v1.size(), v2.size());
            ASSERT_FLOAT_EQ(0.0, (v1-v2).norm());
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
    SolverPtr solver = create("LLT");
    VectorF rhs = VectorF::Ones(m_matrix.rows());

    solver->analyze_pattern(m_matrix);
    solver->factorize(m_matrix);
    VectorF sol = solver->solve(rhs);

    ASSERT_VECTOR_EQ(rhs, sol);
}

TEST_F(SparseSolverTest, LDLT) {
    SolverPtr solver = create("LDLT");
    VectorF rhs = VectorF::Ones(m_matrix.rows());

    solver->analyze_pattern(m_matrix);
    solver->factorize(m_matrix);
    VectorF sol = solver->solve(rhs);

    ASSERT_VECTOR_EQ(rhs, sol);
}

TEST_F(SparseSolverTest, CG) {
    SolverPtr solver = create("CG");
    VectorF rhs = VectorF::Ones(m_matrix.rows());

    solver->analyze_pattern(m_matrix);
    solver->factorize(m_matrix);
    VectorF sol = solver->solve(rhs);

    ASSERT_VECTOR_EQ(rhs, sol);
}

TEST_F(SparseSolverTest, SparseLU) {
    SolverPtr solver = create("SparseLU");
    VectorF rhs = VectorF::Ones(m_matrix.rows());

    solver->analyze_pattern(m_matrix);
    solver->factorize(m_matrix);
    VectorF sol = solver->solve(rhs);

    ASSERT_VECTOR_EQ(rhs, sol);
}

TEST_F(SparseSolverTest, UmfPackLU) {
    SolverPtr solver = create("UmfPackLU");
    VectorF rhs = VectorF::Ones(m_matrix.rows());

    solver->analyze_pattern(m_matrix);
    solver->factorize(m_matrix);
    VectorF sol = solver->solve(rhs);

    ASSERT_VECTOR_EQ(rhs, sol);
}
