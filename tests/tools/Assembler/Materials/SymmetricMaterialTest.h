/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/SymmetricMaterial.h>

#include "MaterialTest.h"

class SymmetricMaterialTest : public MaterialTest {
    protected:
        MaterialPtr create_identity_material_tensor(size_t dim) {
            size_t flattened_size = dim * (dim+1) / 2;
            MatrixF identity = MatrixF::Identity(flattened_size, flattened_size);
            // We need multiply these entries by 0.5 because the matrix maps
            // engineer strain to stress.
            for (size_t i=dim; i<flattened_size; i++) {
                identity(i,i) *= 0.5;
            }
            return MaterialPtr(new SymmetricMaterial(m_density, identity));
        }

        MaterialPtr create_general(size_t dim) {
            size_t flattened_size = dim * (dim+1) / 2;
            MatrixF tensor(flattened_size, flattened_size);
            for (size_t i=0; i<flattened_size; i++) {
                for (size_t j=0; j<flattened_size; j++) {
                    tensor(i,j) = i+j+1;
                }
            }
            return MaterialPtr(new SymmetricMaterial(m_density, tensor));
        }

};

TEST_F(SymmetricMaterialTest, 2D) {
    MaterialPtr mat = create_identity_material_tensor(2);
    assert_material_symmetry(2, mat);
    MatrixF strain(2, 2);
    strain << 1, 2,
              2, 3;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);
    assert_matrix_eq(strain, stress);
}

TEST_F(SymmetricMaterialTest, 3D) {
    MaterialPtr mat = create_identity_material_tensor(3);
    assert_material_symmetry(3, mat);
    MatrixF strain(3, 3);
    strain << 1, 4, 5,
              4, 2, 6,
              5, 6, 3;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);
    assert_matrix_eq(strain, stress);
}

TEST_F(SymmetricMaterialTest, Symmetry) {
    MaterialPtr mat_2D = create_general(2);
    assert_material_symmetry(2, mat_2D);
    MaterialPtr mat_3D = create_general(3);
    assert_material_symmetry(3, mat_3D);
}

TEST_F(SymmetricMaterialTest, Energy2D) {
    MaterialPtr mat = create_general(2);

    MatrixF strain(2, 2);
    strain << 1.0, 0.2,
              0.2, 2.0;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);
    Float energy = (strain.array() * stress.array()).sum();

    Float true_energy = 0.0;
    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<2; j++) {
            for (size_t k=0; k<2; k++) {
                for (size_t l=0; l<2; l++) {
                    true_energy += mat->get_material_tensor(i,j,k,l, m_origin)
                        * strain(i,j) * strain(k,l);
                }
            }
        }
    }

    ASSERT_FLOAT_EQ(true_energy, energy);
}

TEST_F(SymmetricMaterialTest, Energy3D) {
    MaterialPtr mat = create_general(3);

    MatrixF strain(3, 3);
    strain << 1.0, 0.2, 0.3,
              0.2, 2.0, 0.4,
              0.3, 0.4, 3.0;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);
    Float energy = (strain.array() * stress.array()).sum();

    Float true_energy = 0.0;
    for (size_t i=0; i<3; i++) {
        for (size_t j=0; j<3; j++) {
            for (size_t k=0; k<3; k++) {
                for (size_t l=0; l<3; l++) {
                    true_energy += mat->get_material_tensor(i,j,k,l, m_origin)
                        * strain(i,j) * strain(k,l);
                }
            }
        }
    }

    ASSERT_FLOAT_EQ(true_energy, energy);
}

