/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "AssemblerTest.h"

#include <Assembler/Assemblers/DisplacementStrainAssembler.h>

class DisplacementStrainAssemblerTest : public AssemblerTest {
    protected:
        virtual void init_assembler() {
            m_assembler = Assembler::Ptr(new DisplacementStrainAssembler());
        }
};

TEST_F(DisplacementStrainAssemblerTest, TetSize) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix B = m_assembler->assemble(setting);
    ASSERT_EQ(6, B.rows());
    ASSERT_EQ(12, B.cols());
}

TEST_F(DisplacementStrainAssemblerTest, SquareSize) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix B = m_assembler->assemble(setting);
    ASSERT_EQ(6, B.rows());
    ASSERT_EQ(8, B.cols());
}

