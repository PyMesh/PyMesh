/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#include "AssemblerTest.h"

#include <Assembler/Assemblers/GradientAssembler.h>

class GradientAssemblerTest : public AssemblerTest {
    protected:
        virtual void init_assembler() {
            m_assembler = Assembler::Ptr(new GradientAssembler());
        }
};

TEST_F(GradientAssemblerTest, TetSize) {
    auto setting = load_setting("tet.msh");
    ZSparseMatrix G = m_assembler->assemble(setting);
    ASSERT_EQ(3, G.rows());
    ASSERT_EQ(4, G.cols());
}

TEST_F(GradientAssemblerTest, SquareSize) {
    auto setting = load_setting("square_2D.obj");
    ZSparseMatrix G = m_assembler->assemble(setting);
    ASSERT_EQ(4, G.rows());
    ASSERT_EQ(4, G.cols());
}

TEST_F(GradientAssemblerTest, Ball) {
    auto setting = load_setting("ball.msh");
    auto mesh = setting->get_mesh();
    ZSparseMatrix G = m_assembler->assemble(setting);
    ASSERT_EQ(mesh->getNbrElements() * mesh->getDim(), G.rows());
    ASSERT_EQ(mesh->getNbrNodes(), G.cols());

    auto v = VectorF::Ones(mesh->getNbrNodes());
    auto r = G * v;
    ASSERT_NEAR(0.0, r.minCoeff(), 1e-12);
    ASSERT_NEAR(0.0, r.maxCoeff(), 1e-12);
}

