/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "AssemblerTest.h"

#include <Assembler/Assemblers/MassAssembler.h>

class MassAssemblerTest : public AssemblerTest {
    protected:
        virtual void init_assembler() {
            m_assembler = Assembler::Ptr(new MassAssembler());
        }
};

TEST_F(MassAssemblerTest, TetSize) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix M = m_assembler->assemble(setting);
    ASSERT_EQ(4, M.rows());
    ASSERT_EQ(4, M.cols());
}

TEST_F(MassAssemblerTest, SquareSize) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix M = m_assembler->assemble(setting);
    ASSERT_EQ(4, M.rows());
    ASSERT_EQ(4, M.cols());
}

TEST_F(MassAssemblerTest, TetPositiveEntries) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix M = m_assembler->assemble(setting);
    size_t nz_entries = M.get_value_size();
    ASSERT_GT(nz_entries, 0);
    const Float* values = M.valuePtr();
    for (size_t i=0; i<nz_entries; i++) {
        ASSERT_GT(values[i], 0.0);
    }
}

TEST_F(MassAssemblerTest, SquarePositiveEntries) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix M = m_assembler->assemble(setting);
    size_t nz_entries = M.get_value_size();
    ASSERT_GT(nz_entries, 0);
    const Float* values = M.valuePtr();
    for (size_t i=0; i<nz_entries; i++) {
        ASSERT_GT(values[i], 0.0);
    }
}

