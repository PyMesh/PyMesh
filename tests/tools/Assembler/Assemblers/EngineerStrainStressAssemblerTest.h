/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "AssemblerTest.h"

#include <Assembler/Assemblers/EngineerStrainStressAssembler.h>

class EngineerStrainStressAssemblerTest : public AssemblerTest {
    protected:
        virtual void init_assembler() {
            m_assembler = Assembler::Ptr(new EngineerStrainStressAssembler());
        }
};

TEST_F(EngineerStrainStressAssemblerTest, TetSize) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix C = m_assembler->assemble(setting);
    ASSERT_EQ(6, C.rows());
    ASSERT_EQ(6, C.cols());
}

TEST_F(EngineerStrainStressAssemblerTest, SquareSize) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix C = m_assembler->assemble(setting);
    ASSERT_EQ(6, C.rows());
    ASSERT_EQ(6, C.cols());
}
