#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/IsotropicMaterial.h>

class IsotropicMaterialTest : public ::testing::Test {
    protected:
        typedef Material::Ptr MaterialPtr;

        virtual void SetUp() {
            m_density = 1.0;
            m_origin = VectorF::Zero(3);
            m_ones = VectorF::Ones(3);
        }

        MaterialPtr create(size_t dim) {
            m_young = 1.0;
            m_poisson = 0.0;
            if (dim == 2) {
                return MaterialPtr(
                        new IsotropicMaterial<2>(m_density, m_young, m_poisson));
            } else if (dim == 3) {
                return MaterialPtr(
                        new IsotropicMaterial<3>(m_density, m_young, m_poisson));
            }
        }

    protected:
        Float m_density;
        VectorF m_origin;
        VectorF m_ones;
        Float m_young;
        Float m_poisson;
};

TEST_F(IsotropicMaterialTest, 3D) {
    MaterialPtr mat = create(3);
    for (size_t i=0; i<3; i++) {
        for (size_t j=0; j<3; j++) {
            for (size_t k=0; k<3; k++) {
                for (size_t l=0; l<3; l++) {
                    if (i == k && j == l) {
                        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,j,k,l, m_origin));
                    } else {
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
                    if (i == k && j == l) {
                        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,j,k,l, m_origin));
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

