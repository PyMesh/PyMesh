/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "AssemblerTest.h"

#include <Assembler/Assemblers/ElasticityTensorAssembler.h>

class ElasticityTensorAssemblerTest : public AssemblerTest {
    protected:
        virtual void init_assembler() {
            m_assembler = Assembler::Ptr(new ElasticityTensorAssembler());
        }
};

TEST_F(ElasticityTensorAssemblerTest, TetSize) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix B = m_assembler->assemble(setting);
    ASSERT_EQ(6, B.rows());
    ASSERT_EQ(6, B.cols());
}

TEST_F(ElasticityTensorAssemblerTest, SquareSize) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix B = m_assembler->assemble(setting);
    ASSERT_EQ(6, B.rows());
    ASSERT_EQ(6, B.cols());
}

