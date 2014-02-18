#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/Material.h>

class MaterialTest : public ::testing::Test {
    protected:
        typedef Material::Ptr MaterialPtr;

        virtual void SetUp() {
            m_density = 1.0;
            m_origin = VectorF::Zero(3);
            m_ones = VectorF::Ones(3);
        }

    protected:
        Float m_density;
        VectorF m_origin;
        VectorF m_ones;
};

TEST_F(MaterialTest, UniformMaterialCreation3D) {
    MatrixF tensor(9, 9);
    for (size_t i=0; i<9; i++) {
        for (size_t j=0; j<9; j++) {
            tensor(i,j) = i*9+j;
        }
    }

    MaterialPtr mat = Material::create(m_density, tensor);
    ASSERT_FLOAT_EQ(m_density, mat->get_density());
}

TEST_F(MaterialTest, UniformMaterialCreation2D) {
    MatrixF tensor(4, 4);
    for (size_t i=0; i<4; i++) {
        for (size_t j=0; j<4; j++) {
            tensor(i,j) = i*4+j;
        }
    }

    MaterialPtr mat = Material::create(m_density, tensor);
    ASSERT_FLOAT_EQ(m_density, mat->get_density());
}
