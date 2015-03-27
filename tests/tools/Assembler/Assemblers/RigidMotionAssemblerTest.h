/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "AssemblerTest.h"

#include <Assembler/Assemblers/RigidMotionAssembler.h>

class RigidMotionAssemblerTest : public AssemblerTest {
    protected:
        virtual void init_assembler() {
            m_assembler = Assembler::Ptr(new RigidMotionAssembler());
        }
};

TEST_F(RigidMotionAssemblerTest, TetSize) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix Ru = m_assembler->assemble(setting);
    ASSERT_EQ(6, Ru.rows());
    ASSERT_EQ(12, Ru.cols());
}

TEST_F(RigidMotionAssemblerTest, SquareSize) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix Ru = m_assembler->assemble(setting);
    ASSERT_EQ(3, Ru.rows());
    ASSERT_EQ(8, Ru.cols());
}

TEST_F(RigidMotionAssemblerTest, TetTranslation) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix Ru = m_assembler->assemble(setting);
    VectorF translation = VectorF::Ones(Ru.cols());
    VectorF result = Ru * translation;
    const size_t num_vertices = setting->get_mesh()->getNbrNodes();

    ASSERT_FLOAT_EQ(1.0, result[0] / num_vertices);
    ASSERT_FLOAT_EQ(1.0, result[1] / num_vertices);
    ASSERT_FLOAT_EQ(1.0, result[2] / num_vertices);
    ASSERT_NEAR(0.0, result[3], 1e-5);
    ASSERT_NEAR(0.0, result[4], 1e-5);
    ASSERT_NEAR(0.0, result[5], 1e-5);
}

TEST_F(RigidMotionAssemblerTest, SquareTranslation) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix Ru = m_assembler->assemble(setting);
    VectorF translation = VectorF::Ones(Ru.cols());
    VectorF result = Ru * translation;
    const size_t num_vertices = setting->get_mesh()->getNbrNodes();

    ASSERT_FLOAT_EQ(1.0, result[0] / num_vertices);
    ASSERT_FLOAT_EQ(1.0, result[1] / num_vertices);
    ASSERT_NEAR(0.0, result[2], 1e-5);
}

