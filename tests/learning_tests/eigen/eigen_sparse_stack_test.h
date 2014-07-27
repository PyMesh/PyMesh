#pragma once

#include <Core/EigenTypedef.h>
#include <Math/ZSparseMatrix.h>

class EigenSparseStackTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            for (size_t i=0; i<4; i++) {
                ZSparseMatrix* mat = new ZSparseMatrix(3, 3);
                mat->setConstant(i+1);
                m_matrices.push_back(mat);
            }
        }

        virtual void TearDown() {
            for (size_t i=0; i<4; i++) {
                delete m_matrices[i];
            }
        }

    private:
        std::vector<ZSparseMatrix*> m_matrices
};

TEST_F(EigenSparseStackTest, Init) {
}
