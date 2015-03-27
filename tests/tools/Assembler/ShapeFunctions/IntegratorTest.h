/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Assembler/Elements/Elements.h>
#include <Assembler/ShapeFunctions/Integrator.h>
#include <Assembler/ShapeFunctions/ShapeFunction.h>

#include <TestBase.h>

class IntegratorTest : public TestBase {
    protected:
        typedef Elements::Ptr FEMeshPtr;
        typedef Integrator::Ptr IntegratorPtr;
        typedef ShapeFunction::Ptr ShapeFuncPtr;

        FEMeshPtr load_fem_mesh(const std::string& filename) {
            return Elements::adapt(load_mesh(filename));
        }

        IntegratorPtr create_integrator(FEMeshPtr mesh) {
            ShapeFuncPtr shape_func =
                ShapeFunction::create(mesh, "linear");
            IntegratorPtr integrator = Integrator::create(mesh, shape_func);
            return integrator;
        }

        Vector3F convert(const VectorF& v) {
            Vector3F result(0, 0, 0);
            result.segment(0, v.size()) = v;
            return result;
        }

        Float cotan_weight(const Vector3F& v1, const Vector3F& v2,
                const Vector3F& v3) {
            Vector3F a = v2 - v1;
            Vector3F b = v3 - v1;
            return 0.5 * a.dot(b) / a.cross(b).norm();
        }
};

TEST_F(IntegratorTest, FuncOverTriangle) {
    FEMeshPtr mesh = load_fem_mesh("square_2D.obj");
    IntegratorPtr integrator = create_integrator(mesh);
    const size_t num_element = mesh->getNbrElements();
    for (size_t i=0; i<num_element; i++) {
        Float area = mesh->getElementVolume(i);
        // Magical numbers come from Gaussian quadrature pts over triangle.
        ASSERT_FLOAT_EQ(area /  6.0,  integrator->integrate_func(i, 0, 0));
        ASSERT_FLOAT_EQ(area / 12.0,  integrator->integrate_func(i, 0, 1));
        ASSERT_FLOAT_EQ(area / 12.0,  integrator->integrate_func(i, 0, 2));

        ASSERT_FLOAT_EQ(area / 12.0,  integrator->integrate_func(i, 1, 0));
        ASSERT_FLOAT_EQ(area /  6.0,  integrator->integrate_func(i, 1, 1));
        ASSERT_FLOAT_EQ(area / 12.0,  integrator->integrate_func(i, 1, 2));

        ASSERT_FLOAT_EQ(area / 12.0,  integrator->integrate_func(i, 2, 0));
        ASSERT_FLOAT_EQ(area / 12.0,  integrator->integrate_func(i, 2, 1));
        ASSERT_FLOAT_EQ(area /  6.0,  integrator->integrate_func(i, 2, 2));
    }
}

TEST_F(IntegratorTest, FuncOverTetrahedron) {
    FEMeshPtr mesh = load_fem_mesh("cube.msh");
    IntegratorPtr integrator = create_integrator(mesh);
    const size_t num_element = mesh->getNbrElements();
    for (size_t i=0; i<num_element; i++) {
        Float volume = mesh->getElementVolume(i);
        // Magical numbers come from Gaussian quadrature pts over tetrahedron.
        ASSERT_FLOAT_EQ(volume / 10.0, integrator->integrate_func(i, 0, 0));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 0, 1));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 0, 2));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 0, 3));

        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 1, 0));
        ASSERT_FLOAT_EQ(volume / 10.0, integrator->integrate_func(i, 1, 1));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 1, 2));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 1, 3));

        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 2, 0));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 2, 1));
        ASSERT_FLOAT_EQ(volume / 10.0, integrator->integrate_func(i, 2, 2));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 2, 3));

        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 3, 0));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 3, 1));
        ASSERT_FLOAT_EQ(volume / 20.0, integrator->integrate_func(i, 3, 2));
        ASSERT_FLOAT_EQ(volume / 10.0, integrator->integrate_func(i, 3, 3));
    }
}

TEST_F(IntegratorTest, GradOverTriangle) {
    FEMeshPtr mesh = load_fem_mesh("square_2D.obj");
    IntegratorPtr integrator = create_integrator(mesh);
    const size_t num_element = mesh->getNbrElements();
    for (size_t i=0; i<num_element; i++) {
        // Check against cotangent weights.
        VectorI elem = mesh->getElement(i);

        Vector3F v0 = convert(mesh->getNode(elem[0]));
        Vector3F v1 = convert(mesh->getNode(elem[1]));
        Vector3F v2 = convert(mesh->getNode(elem[2]));

        Float cotan_0 = cotan_weight(v0, v1, v2);
        Float cotan_1 = cotan_weight(v1, v2, v0);
        Float cotan_2 = cotan_weight(v2, v0, v1);

        ASSERT_FLOAT_EQ(-cotan_0, integrator->integrate_grad(i, 1, 2));
        ASSERT_FLOAT_EQ(-cotan_1, integrator->integrate_grad(i, 2, 0));
        ASSERT_FLOAT_EQ(-cotan_2, integrator->integrate_grad(i, 0, 1));
    }
}

TEST_F(IntegratorTest, GradOverTetrahedronSymmetry) {
    FEMeshPtr mesh = load_fem_mesh("tet.msh");
    IntegratorPtr integrator = create_integrator(mesh);

    // Check phi_i * phi_j == phi_j * phi_i.
    ASSERT_FLOAT_EQ(integrator->integrate_grad(0, 0, 1), integrator->integrate_grad(0, 1, 0));
    ASSERT_FLOAT_EQ(integrator->integrate_grad(0, 1, 2), integrator->integrate_grad(0, 2, 1));
    ASSERT_FLOAT_EQ(integrator->integrate_grad(0, 2, 3), integrator->integrate_grad(0, 3, 2));
    ASSERT_FLOAT_EQ(integrator->integrate_grad(0, 3, 0), integrator->integrate_grad(0, 0, 3));

    // Tet.msh is plutonic, so results are the same for all vertices.
    ASSERT_NEAR(integrator->integrate_grad(0, 0, 0), integrator->integrate_grad(0, 1, 1), 1e-6);
    ASSERT_NEAR(integrator->integrate_grad(0, 0, 0), integrator->integrate_grad(0, 2, 2), 1e-6);
    ASSERT_NEAR(integrator->integrate_grad(0, 0, 0), integrator->integrate_grad(0, 3, 3), 1e-6);
}

TEST_F(IntegratorTest, MaterialContractionTriangle) {
    FEMeshPtr mesh = load_fem_mesh("square_2D.obj");
    IntegratorPtr integrator = create_integrator(mesh);
    Material::Ptr material = Material::create_isotropic(2, 1.0, 1.0, 0.0);

    const size_t num_element = mesh->getNbrElements();
    for (size_t i=0; i<num_element; i++) {
        MatrixF coeff_00 = integrator->integrate_material_contraction(i, 0, 0, material);
        MatrixF coeff_01 = integrator->integrate_material_contraction(i, 0, 1, material);
        MatrixF coeff_02 = integrator->integrate_material_contraction(i, 0, 2, material);

        MatrixF coeff_10 = integrator->integrate_material_contraction(i, 1, 0, material);
        MatrixF coeff_11 = integrator->integrate_material_contraction(i, 1, 1, material);
        MatrixF coeff_12 = integrator->integrate_material_contraction(i, 1, 2, material);

        MatrixF coeff_20 = integrator->integrate_material_contraction(i, 2, 0, material);
        MatrixF coeff_21 = integrator->integrate_material_contraction(i, 2, 1, material);
        MatrixF coeff_22 = integrator->integrate_material_contraction(i, 2, 2, material);

        ASSERT_EQ(coeff_00, coeff_00.transpose());
        ASSERT_EQ(coeff_01, coeff_10.transpose());
        ASSERT_EQ(coeff_02, coeff_20.transpose());
        ASSERT_EQ(coeff_11, coeff_11.transpose());
        ASSERT_EQ(coeff_12, coeff_21.transpose());
        ASSERT_EQ(coeff_22, coeff_22.transpose());
    }
}

TEST_F(IntegratorTest, MaterialContractionTetrahedron) {
    FEMeshPtr mesh = load_fem_mesh("tet.msh");
    IntegratorPtr integrator = create_integrator(mesh);
    Material::Ptr material = Material::create_isotropic(3, 1.0, 1.0, 0.0);

    const size_t num_element = mesh->getNbrElements();
    for (size_t i=0; i<num_element; i++) {
        MatrixF coeff_00 = integrator->integrate_material_contraction(i, 0, 0, material);
        MatrixF coeff_01 = integrator->integrate_material_contraction(i, 0, 1, material);
        MatrixF coeff_02 = integrator->integrate_material_contraction(i, 0, 2, material);
        MatrixF coeff_03 = integrator->integrate_material_contraction(i, 0, 3, material);

        MatrixF coeff_10 = integrator->integrate_material_contraction(i, 1, 0, material);
        MatrixF coeff_11 = integrator->integrate_material_contraction(i, 1, 1, material);
        MatrixF coeff_12 = integrator->integrate_material_contraction(i, 1, 2, material);
        MatrixF coeff_13 = integrator->integrate_material_contraction(i, 1, 3, material);

        MatrixF coeff_20 = integrator->integrate_material_contraction(i, 2, 0, material);
        MatrixF coeff_21 = integrator->integrate_material_contraction(i, 2, 1, material);
        MatrixF coeff_22 = integrator->integrate_material_contraction(i, 2, 2, material);
        MatrixF coeff_23 = integrator->integrate_material_contraction(i, 2, 3, material);

        MatrixF coeff_30 = integrator->integrate_material_contraction(i, 3, 0, material);
        MatrixF coeff_31 = integrator->integrate_material_contraction(i, 3, 1, material);
        MatrixF coeff_32 = integrator->integrate_material_contraction(i, 3, 2, material);
        MatrixF coeff_33 = integrator->integrate_material_contraction(i, 3, 3, material);

        ASSERT_EQ(coeff_00, coeff_00.transpose());
        ASSERT_EQ(coeff_01, coeff_10.transpose());
        ASSERT_EQ(coeff_02, coeff_20.transpose());
        ASSERT_EQ(coeff_03, coeff_30.transpose());
        ASSERT_EQ(coeff_11, coeff_11.transpose());
        ASSERT_EQ(coeff_12, coeff_21.transpose());
        ASSERT_EQ(coeff_13, coeff_31.transpose());
        ASSERT_EQ(coeff_22, coeff_22.transpose());
        ASSERT_EQ(coeff_23, coeff_32.transpose());
        ASSERT_EQ(coeff_33, coeff_33.transpose());
    }
}
