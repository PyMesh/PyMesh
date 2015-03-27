/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/IsotropicMaterial.h>

#include "MaterialTest.h"

class IsotropicMaterialTest : public MaterialTest {
    protected:
        MaterialPtr create(size_t dim, Float young=1.0, Float poisson=0.0) {
            m_young = young;
            m_poisson = poisson;
            if (dim == 2) {
                return MaterialPtr(
                        new IsotropicMaterial<2>(m_density, m_young, m_poisson));
            } else if (dim == 3) {
                return MaterialPtr(
                        new IsotropicMaterial<3>(m_density, m_young, m_poisson));
            } else {
                std::stringstream err_msg;
                err_msg << "Unsupported dimension: " << dim;
                throw NotImplementedError(err_msg.str());
            }
        }

    protected:
        Float m_young;
        Float m_poisson;
};

TEST_F(IsotropicMaterialTest, 3D) {
    MaterialPtr mat = create(3);
    for (size_t i=0; i<3; i++) {
        for (size_t j=0; j<3; j++) {
            for (size_t k=0; k<3; k++) {
                for (size_t l=0; l<3; l++) {
                    if (i == j && k == l && i == k) {
                        // Corresponding diagonal entries
                        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,j,k,l, m_origin));
                    } else if ((i == k && j == l) || (i == l && j == k)) {
                        // Corresponding off diagonal entries
                        ASSERT_FLOAT_EQ(1.0,
                                mat->get_material_tensor(i,j,k,l, m_origin) +
                                mat->get_material_tensor(i,j,l,k, m_origin));
                    } else {
                        // Non-corresponding entries
                        ASSERT_FLOAT_EQ(0.0, mat->get_material_tensor(i,j,k,l, m_origin));
                    }
                }
            }
        }
    }
}

TEST_F(IsotropicMaterialTest, 2D) {
    MaterialPtr mat = create(2);
    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<2; j++) {
            for (size_t k=0; k<2; k++) {
                for (size_t l=0; l<2; l++) {
                    if (i == j && k == l && i == k) {
                        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,j,k,l, m_origin));
                    } else if ((i == k && j == l) || (i == l && j == k)) {
                        ASSERT_FLOAT_EQ(1.0,
                                mat->get_material_tensor(i,j,k,l, m_origin) +
                                mat->get_material_tensor(i,j,l,k,m_origin));
                    } else {
                        ASSERT_FLOAT_EQ(0.0, mat->get_material_tensor(i,j,k,l, m_origin));
                    }
                }
            }
        }
    }
}

TEST_F(IsotropicMaterialTest, WrongSize) {
    ASSERT_THROW(IsotropicMaterial<1>(m_density, 1.0, 0.0), NotImplementedError);
}

TEST_F(IsotropicMaterialTest, StrainToStress) {
    MaterialPtr mat = create(3);
    MatrixF strain = MatrixF::Identity(3,3);
    MatrixF stress = mat->strain_to_stress(strain, m_origin);

    ASSERT_FLOAT_EQ(strain(0,0), stress(0,0));
    ASSERT_FLOAT_EQ(strain(0,1), stress(0,1));
    ASSERT_FLOAT_EQ(strain(0,2), stress(0,2));

    ASSERT_FLOAT_EQ(strain(1,0), stress(1,0));
    ASSERT_FLOAT_EQ(strain(1,1), stress(1,1));
    ASSERT_FLOAT_EQ(strain(1,2), stress(1,2));

    ASSERT_FLOAT_EQ(strain(2,0), stress(2,0));
    ASSERT_FLOAT_EQ(strain(2,1), stress(2,1));
    ASSERT_FLOAT_EQ(strain(2,2), stress(2,2));
}

TEST_F(IsotropicMaterialTest, InverseTensor2D) {
    // The stress to strain tensor also have a closed formula for the isotropic
    // case.  In this test, we check if it is the inverse of strain to stress
    // tensor.
    Float young = 9.0;
    Float poisson = -0.9;
    MaterialPtr mat = create(2, young, poisson);

    // Formula of S from
    // https://www.efunda.com/formulae/solid_mechanics/mat_mechanics/hooke_isotropic.cfm
    MatrixF S(3, 3);
    S <<     1.0 / young, -poisson / young,                 0.0,
        -poisson / young,      1.0 / young,                 0.0,
                     0.0,              0.0, (1+poisson) / young;

    MatrixF C = MatrixF::Zero(3,3);
    C(0,0) = mat->get_material_tensor(0,0,0,0,m_origin);
    C(1,1) = mat->get_material_tensor(1,1,1,1,m_origin);
    C(2,2) = mat->get_material_tensor(0,1,0,1,m_origin) +
             mat->get_material_tensor(0,1,1,0,m_origin);
    C(0,1) = mat->get_material_tensor(0,0,1,1,m_origin);
    C(1,0) = mat->get_material_tensor(1,1,0,0,m_origin);

    MatrixF I = S * C;
    MatrixF Id = MatrixF::Identity(3, 3);
    assert_matrix_eq(Id, I);
}

TEST_F(IsotropicMaterialTest, InverseTensor3D) {
    Float young = 9.0;
    Float poisson = -0.9;
    MaterialPtr mat = create(3, young, poisson);

    // Formula of S from
    // https://www.efunda.com/formulae/solid_mechanics/mat_mechanics/hooke_isotropic.cfm
    MatrixF S(6, 6);
    S <<     1.0 / young, -poisson / young, -poisson / young,                 0.0, 0.0, 0.0,
        -poisson / young,      1.0 / young, -poisson / young,                 0.0, 0.0, 0.0,
        -poisson / young, -poisson / young,      1.0 / young,                 0.0, 0.0, 0.0,
                     0.0,              0.0,              0.0, (1+poisson) / young, 0.0, 0.0,
                     0.0,              0.0,              0.0, 0.0, (1+poisson) / young, 0.0,
                     0.0,              0.0,              0.0, 0.0, 0.0, (1+poisson) / young;

    MatrixF C = MatrixF::Zero(6,6);
    C(0,0) = mat->get_material_tensor(0,0,0,0,m_origin);
    C(1,1) = mat->get_material_tensor(1,1,1,1,m_origin);
    C(2,2) = mat->get_material_tensor(2,2,2,2,m_origin);
    C(3,3) = mat->get_material_tensor(0,1,0,1,m_origin) +
             mat->get_material_tensor(0,1,1,0,m_origin);
    C(4,4) = mat->get_material_tensor(0,2,0,2,m_origin) +
             mat->get_material_tensor(0,2,2,0,m_origin);
    C(5,5) = mat->get_material_tensor(1,2,1,2,m_origin) +
             mat->get_material_tensor(1,2,2,1,m_origin);
    C(0,1) = mat->get_material_tensor(0,0,1,1,m_origin);
    C(1,0) = mat->get_material_tensor(1,1,0,0,m_origin);
    C(0,2) = mat->get_material_tensor(0,0,2,2,m_origin);
    C(2,0) = mat->get_material_tensor(2,2,0,0,m_origin);
    C(1,2) = mat->get_material_tensor(1,1,2,2,m_origin);
    C(2,1) = mat->get_material_tensor(2,2,1,1,m_origin);

    MatrixF I = S * C;
    MatrixF Id = MatrixF::Identity(6, 6);
    assert_matrix_eq(Id, I);
}

TEST_F(IsotropicMaterialTest, Energy2D) {
    Float young = 0.5;
    Float poisson = 0.3;
    MaterialPtr mat = create(2, young, poisson);

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

TEST_F(IsotropicMaterialTest, Energy3D) {
    Float young = 0.5;
    Float poisson = 0.3;
    MaterialPtr mat = create(3, young, poisson);

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

