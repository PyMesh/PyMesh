#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/SymmetricMaterial.h>

class SymmetricMaterialTest : public ::testing::Test {
    protected:
        typedef Material::Ptr MaterialPtr;

        virtual void SetUp() {
            m_density = 1.0;
            m_origin = VectorF::Zero(3);
            m_ones = VectorF::Ones(3);
        }

        MaterialPtr create(size_t dim) {
            size_t flattened_size = dim * (dim+1) / 2;
            MatrixF identity = MatrixF::Identity(flattened_size, flattened_size);
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

        void ASSERT_MATRIX_EQ(const MatrixF& mat1, const MatrixF& mat2) {
            ASSERT_EQ(mat1.rows(), mat2.rows());
            ASSERT_EQ(mat1.cols(), mat2.cols());
            for (size_t i=0; i<mat1.rows(); i++) {
                for (size_t j=0; j<mat1.cols(); j++) {
                    ASSERT_FLOAT_EQ(mat1(i,j), mat2(i,j));
                }
            }
        }

        void ASSERT_SYMMETRY(size_t dim, MaterialPtr mat) {
            for (size_t i=0; i<dim; i++) {
                for (size_t j=0; j<dim; j++) {
                    for (size_t k=0; k<dim; k++) {
                        for (size_t l=0; l<dim; l++) {
                            ASSERT_FLOAT_EQ(
                                    mat->get_material_tensor(i,j,k,l, m_origin),
                                    mat->get_material_tensor(j,i,k,l, m_origin));
                            ASSERT_FLOAT_EQ(
                                    mat->get_material_tensor(i,j,k,l, m_origin),
                                    mat->get_material_tensor(i,j,l,k, m_origin));
                            ASSERT_FLOAT_EQ(
                                    mat->get_material_tensor(i,j,k,l, m_origin),
                                    mat->get_material_tensor(k,l,i,j, m_origin));
                        }
                    }
                }
            }
        }

    protected:
        Float m_density;
        VectorF m_origin;
        VectorF m_ones;
};

TEST_F(SymmetricMaterialTest, 2D) {
    MaterialPtr mat = create(2);
    ASSERT_SYMMETRY(2, mat);
    MatrixF strain(2, 2);
    strain << 1, 2,
              2, 3;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);
    ASSERT_MATRIX_EQ(strain, stress);
}

TEST_F(SymmetricMaterialTest, 3D) {
    MaterialPtr mat = create(3);
    ASSERT_SYMMETRY(3, mat);
    MatrixF strain(3, 3);
    strain << 1, 4, 5,
              4, 2, 6,
              5, 6, 3;
    MatrixF stress = mat->strain_to_stress(strain, m_origin);
    ASSERT_MATRIX_EQ(strain, stress);
}

TEST_F(SymmetricMaterialTest, Symmetry) {
    MaterialPtr mat_2D = create_general(2);
    ASSERT_SYMMETRY(2, mat_2D);
    MaterialPtr mat_3D = create_general(3);
    ASSERT_SYMMETRY(3, mat_3D);
}

