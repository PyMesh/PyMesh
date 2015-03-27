/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/UniformMaterial.h>

#include "MaterialTest.h"

class UniformMaterialTest : public MaterialTest {
    protected:
        void init_material_tensor(size_t dim) {
            size_t tensor_size = dim*dim;
            m_material_tensor = MatrixF(tensor_size, tensor_size);
            for (size_t i=0; i<tensor_size; i++) {
                for (size_t j=0; j<tensor_size; j++) {
                    m_material_tensor(i,j) = i*tensor_size+j;
                }
            }
        }

        MaterialPtr create(size_t dim) {
            if (dim == 2 || dim == 3) {
                init_material_tensor(dim);
            } else {
                throw RuntimeError("Unknown dim.");
            }
            return MaterialPtr(new UniformMaterial(
                        m_density, m_material_tensor));
        }

    protected:
        MatrixF m_material_tensor;
};

TEST_F(UniformMaterialTest, 3D) {
    MaterialPtr mat = create(3);
    for (size_t i=0; i<3; i++) {
        for (size_t j=0; j<3; j++) {
            for (size_t k=0; k<3; k++) {
                for (size_t l=0; l<3; l++) {
                    Float expected = m_material_tensor(i*3+k, j*3+l);
                    Float actual_0 = mat->get_material_tensor(i,j,k,l,m_origin);
                    Float actual_1 = mat->get_material_tensor(i,j,k,l,m_ones);
                    ASSERT_FLOAT_EQ(expected, actual_0);
                    ASSERT_FLOAT_EQ(expected, actual_1);
                }
            }
        }
    }
    ASSERT_FLOAT_EQ(m_density, mat->get_density());
    ASSERT_FLOAT_EQ(m_density, mat->get_density(m_origin));
}

TEST_F(UniformMaterialTest, 2D) {
    MaterialPtr mat = create(2);
    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<2; j++) {
            for (size_t k=0; k<2; k++) {
                for (size_t l=0; l<2; l++) {
                    Float expected = m_material_tensor(i*2+k, j*2+l);
                    Float actual_0 = mat->get_material_tensor(i,j,k,l,m_origin);
                    Float actual_1 = mat->get_material_tensor(i,j,k,l,m_ones);
                    ASSERT_FLOAT_EQ(expected, actual_0);
                    ASSERT_FLOAT_EQ(expected, actual_1);
                }
            }
        }
    }
    ASSERT_FLOAT_EQ(m_density, mat->get_density());
    ASSERT_FLOAT_EQ(m_density, mat->get_density(m_origin));
}

TEST_F(UniformMaterialTest, WrongSize) {
    MatrixF tensor(1,1);
    ASSERT_THROW(UniformMaterial(m_density, tensor), NotImplementedError);
}

TEST_F(UniformMaterialTest, StrainToStress) {
    MatrixF tensor = MatrixF::Zero(9, 9);
    for (size_t i=0; i<9; i++) {
        tensor(i, i) = 1.0;
    }
    UniformMaterial mat(m_density, tensor);

    MatrixF strain = MatrixF::Ones(3,3);
    MatrixF stress = mat.strain_to_stress(strain, m_origin);
    ASSERT_EQ(3, stress.rows());
    ASSERT_EQ(3, stress.cols());

    ASSERT_FLOAT_EQ(3.0, stress(0,0));
    ASSERT_FLOAT_EQ(0.0, stress(0,1));
    ASSERT_FLOAT_EQ(0.0, stress(0,2));

    ASSERT_FLOAT_EQ(0.0, stress(1,0));
    ASSERT_FLOAT_EQ(3.0, stress(1,1));
    ASSERT_FLOAT_EQ(0.0, stress(1,2));

    ASSERT_FLOAT_EQ(0.0, stress(2,0));
    ASSERT_FLOAT_EQ(0.0, stress(2,1));
    ASSERT_FLOAT_EQ(3.0, stress(2,2));
}

