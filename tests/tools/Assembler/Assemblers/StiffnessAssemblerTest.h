/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "AssemblerTest.h"

#include <Assembler/Assemblers/StiffnessAssembler.h>

class StiffnessAssemblerTest : public AssemblerTest {
    protected:
        virtual void init_assembler() {
            m_assembler = Assembler::Ptr(new StiffnessAssembler());
        }
};

TEST_F(StiffnessAssemblerTest, TetSize) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix K = m_assembler->assemble(setting);
    ASSERT_EQ(12, K.rows());
    ASSERT_EQ(12, K.cols());
}

TEST_F(StiffnessAssemblerTest, SquareSize) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix K = m_assembler->assemble(setting);
    ASSERT_EQ(8, K.rows());
    ASSERT_EQ(8, K.cols());
}

TEST_F(StiffnessAssemblerTest, TetPositiveDiagonal) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix K = m_assembler->assemble(setting);
    for (size_t i=0; i<K.rows(); i++) {
        ASSERT_GT(K.coeff(i, i), 0.0);
    }
}

TEST_F(StiffnessAssemblerTest, SquarePositiveDiagonal) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix K = m_assembler->assemble(setting);
    for (size_t i=0; i<K.rows(); i++) {
        ASSERT_GT(K.coeff(i, i), 0.0);
    }
}



