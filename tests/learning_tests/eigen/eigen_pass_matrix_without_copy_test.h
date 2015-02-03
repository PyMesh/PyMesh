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
