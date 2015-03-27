/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/OrthotropicMaterial.h>

#include "MaterialTest.h"

class OrthotropicMaterialTest : public MaterialTest {
    protected:
        MaterialPtr create(size_t dim) {
            size_t flattened_size = dim * (dim+1) / 2;

            size_t num_young = dim;
            size_t num_poisson = dim*(dim-1);
            size_t num_shear = dim*(dim-1)/2;

            VectorF young = VectorF::Ones(num_young);
            VectorF poisson = VectorF::Zero(num_poisson);
            VectorF shear = VectorF::Ones(num_shear) * 0.5;

            if (dim == 2) {
                return MaterialPtr(new OrthotropicMaterial<2>(m_density, young, poisson, shear));
            } else if (dim == 3) {
                return MaterialPtr(new OrthotropicMaterial<3>(m_density, young, poisson, shear));
            } else {
                std::stringstream err_msg;
                err_msg << "Unsupported dimension: " << dim;
                throw NotImplementedError(err_msg.str());
            }
        }
};

TEST_F(OrthotropicMaterialTest, 2D) {
    MaterialPtr mat = create(2);
    MatrixF strain(2, 2);
    strain << 1, 2,
              2, 3;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);

    assert_matrix_eq(strain, stress);
}

TEST_F(OrthotropicMaterialTest, 3D) {
    MaterialPtr mat = create(3);
    MatrixF strain(3, 3);
    strain << 1, 4, 5,
              4, 2, 6,
              5, 6, 3;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);

    assert_matrix_eq(strain, stress);
}

TEST_F(OrthotropicMaterialTest, Energy2D) {
    VectorF young(2);
    VectorF shear(1);
    VectorF poisson(2);

    young << 1.0, 1.0;
    shear << 2.0;
    poisson << 0.5, 0.5;

    MaterialPtr mat = MaterialPtr(new OrthotropicMaterial<2>(m_density, young, poisson, shear));

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

TEST_F(OrthotropicMaterialTest, Energy3D) {
    VectorF young(3);
    VectorF shear(3);
    VectorF poisson(6);

    young << 1.0, 1.0, 1.0;
    shear << 0.5, 0.9, 1.2;
    poisson << 0.3, 0.3, 0.3, 0.3, 0.3, 0.3;

    MaterialPtr mat = MaterialPtr(new OrthotropicMaterial<3>(m_density, young, poisson, shear));

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

TEST_F(OrthotropicMaterialTest, Symmetry2D) {
    VectorF young(2);
    VectorF shear(1);
    VectorF poisson(2);

    young << 1.0, 0.5;
    shear << 2.0;
    poisson << 1.0, 0.5;

    MaterialPtr mat = MaterialPtr(new OrthotropicMaterial<2>(m_density, young, poisson, shear));
    assert_material_symmetry(2, mat);
}

TEST_F(OrthotropicMaterialTest, Symmetry3D) {
    VectorF young(3);
    VectorF shear(3);
    VectorF poisson(6);

    young << 1.0, 0.5, 0.25;
    shear << 2.0, 1.0, 0.5;
    poisson <<
        0.50, 0.25, // nu_yz, nu_zy
        0.25, 1.00, // nu_zx, nu_xz
        1.00, 0.50; // nu_xy, nu_yx

    MaterialPtr mat = MaterialPtr(new OrthotropicMaterial<3>(m_density, young, poisson, shear));
    assert_material_symmetry(3, mat);
}
