/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/Material.h>

#include <TestBase.h>

class MaterialTest : public TestBase {
    protected:
        typedef Material::Ptr MaterialPtr;

        virtual void SetUp() {
            TestBase::SetUp();
            m_density = 1.0;
            m_origin = VectorF::Zero(3);
            m_ones = VectorF::Ones(3);
        }

        void assert_material_eq(MaterialPtr mat1, MaterialPtr mat2, size_t dim, const VectorF& coord) {
            for (size_t i=0; i<dim; i++) {
                for (size_t j=0; j<dim; j++) {
                    for (size_t k=0; k<dim; k++) {
                        for (size_t l=0; l<dim; l++) {
                            ASSERT_FLOAT_EQ(
                                    mat1->get_material_tensor(i,j,k,l,coord),
                                    mat2->get_material_tensor(i,j,k,l,coord));
                        }
                    }
                }
            }
        }

        void assert_material_symmetry(size_t dim, MaterialPtr mat) {
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

        void assert_matrix_eq(const MatrixF& m1, const MatrixF& m2) {
            ASSERT_EQ(m1.rows(), m2.rows());
            ASSERT_EQ(m1.cols(), m2.cols());
            const size_t rows = m1.rows();
            const size_t cols = m1.cols();
            for (size_t i=0; i<rows; i++) {
                for (size_t j=0; j<cols; j++) {
                    ASSERT_NEAR(m1(i,j), m2(i,j), 1e-6);
                }
            }
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
    for (size_t i=0; i<3; i++) {
        size_t row = i*3+i;
        size_t col = i*3+i;
        ASSERT_FLOAT_EQ(row*9+col, mat->get_material_tensor(i,i,i,i, m_origin));
    }
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
    for (size_t i=0; i<2; i++) {
        size_t row = i*2+i;
        size_t col = i*2+i;
        ASSERT_FLOAT_EQ(row*4+col, mat->get_material_tensor(i,i,i,i, m_origin));
    }
}

TEST_F(MaterialTest, IsotropicMaterialCreation3D) {
    Float young = 1.0;
    Float poisson = 0.0;
    MaterialPtr mat = Material::create_isotropic(3, m_density, young, poisson);
    ASSERT_FLOAT_EQ(m_density, mat->get_density());
    for (size_t i=0; i<3; i++) {
        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,i,i,i, m_origin));
    }
}

TEST_F(MaterialTest, IsotropicMaterialCreation2D) {
    Float young = 1.0;
    Float poisson = 0.0;
    MaterialPtr mat = Material::create_isotropic(2, m_density, young, poisson);
    ASSERT_FLOAT_EQ(m_density, mat->get_density());
    for (size_t i=0; i<2; i++) {
        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,i,i,i, m_origin));
    }
}

TEST_F(MaterialTest, SymmetricMaterialCreation3D) {
    MatrixF tensor(6, 6);
    for (size_t i=0; i<6; i++) {
        for (size_t j=i; j<6; j++) {
            tensor(i,j) = i*j;
            tensor(j,i) = i*j;
        }
    }
    MaterialPtr mat = Material::create_symmetric(m_density, tensor);
    for (size_t i=0; i<3; i++) {
        ASSERT_FLOAT_EQ(Float(i*i), mat->get_material_tensor(i,i,i,i, m_origin));
    }
}

TEST_F(MaterialTest, SymmetricMaterialCreation2D) {
    MatrixF tensor(3, 3);
    for (size_t i=0; i<3; i++) {
        for (size_t j=i; j<3; j++) {
            tensor(i,j) = i*j;
            tensor(j,i) = i*j;
        }
    }
    MaterialPtr mat = Material::create_symmetric(m_density, tensor);
    for (size_t i=0; i<2; i++) {
        ASSERT_FLOAT_EQ(Float(i*i), mat->get_material_tensor(i,i,i,i, m_origin));
    }
}

TEST_F(MaterialTest, OrthotropicMaterialCreation3D) {
    const size_t dim = 3;
    size_t num_young = dim;
    size_t num_poisson = dim*(dim-1);
    size_t num_shear = dim*(dim-1)/2;
    VectorF young = VectorF::Ones(num_young);
    VectorF poisson = VectorF::Zero(num_poisson);
    VectorF shear = VectorF::Ones(num_shear);

    MaterialPtr mat = Material::create_orthotropic(m_density, young, poisson, shear);
    for (size_t i=0; i<dim; i++) {
        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,i,i,i, m_origin));
    }
}

TEST_F(MaterialTest, OrthotropicMaterialCreation2D) {
    const size_t dim = 2;
    size_t num_young = dim;
    size_t num_poisson = dim*(dim-1);
    size_t num_shear = dim*(dim-1)/2;
    VectorF young = VectorF::Ones(num_young);
    VectorF poisson = VectorF::Zero(num_poisson);
    VectorF shear = VectorF::Ones(num_shear);

    MaterialPtr mat = Material::create_orthotropic(m_density, young, poisson, shear);
    for (size_t i=0; i<dim; i++) {
        ASSERT_FLOAT_EQ(1.0, mat->get_material_tensor(i,i,i,i, m_origin));
    }
}
