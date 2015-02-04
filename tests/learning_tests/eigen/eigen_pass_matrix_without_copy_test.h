#pragma once

#include <Core/EigenTypedef.h>

class EigenPassMatrixWithoutCopyTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            m_matrix = MatrixFr::Zero(3, 3);
        }

        template<typename Derived>
        void pass1(Eigen::MatrixBase<Derived>& M) {
            M(0, 0) += 1;
        }

        void pass2(Eigen::Ref<MatrixFr> M) {
            M(0, 0) += 1;
        }

        void pass3(MatrixFr& M) {
            M(0, 0) += 1;
        }

        template<typename Derived>
        void chain_11(Eigen::MatrixBase<Derived>& M) {
            pass1(M);
        }

        template<typename Derived>
        void chain_12(Eigen::MatrixBase<Derived>& M) {
            pass2(M);
        }

        template<typename Derived>
        void chain_13(Eigen::MatrixBase<Derived>& M) {
            pass3(M);
        }

        void chain_21(Eigen::Ref<MatrixFr> M) {
            pass1(M);
        }

        void chain_22(Eigen::Ref<MatrixFr> M) {
            pass2(M);
        }

        void chain_23(Eigen::Ref<MatrixFr> M) {
            // The following does not compile
            //pass3(M);
        }

        void chain_31(MatrixFr& M) {
            pass1(M);
        }

        void chain_32(MatrixFr& M) {
            pass2(M);
        }

        void chain_33(MatrixFr& M) {
            pass3(M);
        }

    protected:
        MatrixFr m_matrix;
};

TEST_F(EigenPassMatrixWithoutCopyTest, PassMatrixFr) {
    ASSERT_FLOAT_EQ(0.0, m_matrix(0, 0));

    pass1(m_matrix);
    ASSERT_FLOAT_EQ(1.0, m_matrix(0, 0));

    pass2(m_matrix);
    ASSERT_FLOAT_EQ(2.0, m_matrix(0, 0));

    pass3(m_matrix);
    ASSERT_FLOAT_EQ(3.0, m_matrix(0, 0));
}

TEST_F(EigenPassMatrixWithoutCopyTest, PassTmpMatrixFr) {
    // The following expression does not compile
    //pass1(MatrixFr::Zero(2, 2));

    // The following expression failed because the argument is read only.
    //pass2(MatrixFr::Zero(2, 2));

    // The following expression does not compile
    //pass3(MatrixFr::Zero(2, 2));
}

TEST_F(EigenPassMatrixWithoutCopyTest, PassRowVector) {
    ASSERT_FLOAT_EQ(0.0, m_matrix(0, 0));

    // The following expresssion does not compile
    //pass1(m_matrix.row(0));
    //ASSERT_FLOAT_EQ(1.0, m_matrix(0, 0));

    pass2(m_matrix.row(0));
    ASSERT_FLOAT_EQ(1.0, m_matrix(0, 0));

    // The following expression does not compile
    //pass3(m_matrix.row(0));
    //ASSERT_FLOAT_EQ(3.0, m_matrix(0, 0));
}

TEST_F(EigenPassMatrixWithoutCopyTest, PassWrongStorageOrder) {
    MatrixF m = MatrixF::Zero(2, 2);
    ASSERT_FLOAT_EQ(0.0, m(0, 0));

    pass1(m);
    ASSERT_FLOAT_EQ(1.0, m(0, 0));

    // The following expression does not compile
    //pass2(m);
    //ASSERT_FLOAT_EQ(2.0, m(0, 0));

    // The following expression does not compile
    //pass3(m);
    //ASSERT_FLOAT_EQ(3.0, m(0, 0));
}

TEST_F(EigenPassMatrixWithoutCopyTest, PassVector) {
    VectorF v = VectorF::Zero(3);
    ASSERT_FLOAT_EQ(0.0, v(0, 0));

    pass1(v);
    ASSERT_FLOAT_EQ(1.0, v(0, 0));

    pass2(v);
    ASSERT_FLOAT_EQ(2.0, v(0, 0));

    // The following expression does not compile
    //pass3(v);
    //ASSERT_FLOAT_EQ(3.0, v(0, 0));
}

TEST_F(EigenPassMatrixWithoutCopyTest, PassMap) {
    auto m = Eigen::Map<MatrixFr>(m_matrix.data(),
            m_matrix.rows(), m_matrix.cols());

    ASSERT_FLOAT_EQ(0.0, m(0, 0));

    pass1(m);
    ASSERT_FLOAT_EQ(1.0, m(0, 0));

    pass2(m);
    ASSERT_FLOAT_EQ(2.0, m(0, 0));

    // The following expression does not compile
    //pass3(m);
    //ASSERT_FLOAT_EQ(3.0, m(0, 0));
}

TEST_F(EigenPassMatrixWithoutCopyTest, Chain1) {
    ASSERT_FLOAT_EQ(0.0, m_matrix(0, 0));

    chain_11(m_matrix);
    ASSERT_FLOAT_EQ(1.0, m_matrix(0, 0));

    // The following expression does not compile
    //chain_12(m_matrix);
    //ASSERT_FLOAT_EQ(2.0, m_matrix(0, 0));

    // The following expression does not compile
    //chain_13(m_matrix);
    //ASSERT_FLOAT_EQ(2.0, m_matrix(0, 0));
}

TEST_F(EigenPassMatrixWithoutCopyTest, Chain2) {
    ASSERT_FLOAT_EQ(0.0, m_matrix(0, 0));

    chain_21(m_matrix);
    ASSERT_FLOAT_EQ(1.0, m_matrix(0, 0));

    chain_22(m_matrix);
    ASSERT_FLOAT_EQ(2.0, m_matrix(0, 0));

    // The following expression does not compile
    //chain_23(m_matrix);
    //ASSERT_FLOAT_EQ(3.0, m_matrix(0, 0));
}

TEST_F(EigenPassMatrixWithoutCopyTest, Chain3) {
    ASSERT_FLOAT_EQ(0.0, m_matrix(0, 0));

    chain_31(m_matrix);
    ASSERT_FLOAT_EQ(1.0, m_matrix(0, 0));

    chain_32(m_matrix);
    ASSERT_FLOAT_EQ(2.0, m_matrix(0, 0));

    chain_33(m_matrix);
    ASSERT_FLOAT_EQ(3.0, m_matrix(0, 0));
}
