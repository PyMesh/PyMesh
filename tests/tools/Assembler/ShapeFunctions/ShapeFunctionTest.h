/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Assembler/Elements/Elements.h>
#include <Assembler/ShapeFunctions/ShapeFunction.h>

#include <TestBase.h>

class ShapeFunctionTest : public TestBase {
    protected:
        typedef Elements::Ptr FEMeshPtr;
        typedef ShapeFunction::Ptr ShapeFuncPtr;

        FEMeshPtr load_fem_mesh(const std::string& filename) {
            return Elements::adapt(load_mesh(filename));
        }

        Float get_edge_length(const VectorF& v1, const VectorF& v2) {
            return (v1 - v2).norm();
        }

        Float get_triangle_area(
                const Vector3F& v1, const Vector3F& v2, const Vector3F& v3) {
            return ((v2 - v1).cross(v3 - v1)).norm() * 0.5;
        }

        Float compute_triangle_height(FEMeshPtr mesh, size_t elem_idx,
                size_t local_node_idx) {
            VectorI face = mesh->getElement(elem_idx);
            Float area = mesh->getElementVolume(elem_idx);
            size_t opp_1 = (local_node_idx + 1) % 3;
            size_t opp_2 = (local_node_idx + 2) % 3;
            VectorF opp_v1 = mesh->getNode(face[opp_1]);
            VectorF opp_v2 = mesh->getNode(face[opp_2]);
            Float base_length = get_edge_length(opp_v1, opp_v2);
            return area * 2.0 / base_length;
        }

        Float compute_tetrahedron_height(FEMeshPtr mesh, size_t elem_idx,
                size_t local_node_idx) {
            VectorI voxel = mesh->getElement(elem_idx);
            Float volume = mesh->getElementVolume(elem_idx);
            size_t opp_1 = (local_node_idx + 1) % 4;
            size_t opp_2 = (local_node_idx + 2) % 4;
            size_t opp_3 = (local_node_idx + 3) % 4;
            VectorF opp_v1 = mesh->getNode(voxel[opp_1]);
            VectorF opp_v2 = mesh->getNode(voxel[opp_2]);
            VectorF opp_v3 = mesh->getNode(voxel[opp_3]);
            Float base_area = get_triangle_area(opp_v1, opp_v2, opp_v3);
            return volume * 3.0 / base_area;
        }
};

TEST_F(ShapeFunctionTest, EvalAtNodeSquare) {
    FEMeshPtr mesh = load_fem_mesh("square_2D.obj");
    ShapeFuncPtr shape_func =
        ShapeFunction::create(mesh, "linear");
    ASSERT_EQ(1.0, shape_func->evaluate_func(0, 0, Vector3F(1, 0, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 1, Vector3F(1, 0, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 2, Vector3F(1, 0, 0)));

    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 0, Vector3F(0, 1, 0)));
    ASSERT_EQ(1.0, shape_func->evaluate_func(0, 1, Vector3F(0, 1, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 2, Vector3F(0, 1, 0)));

    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 0, Vector3F(0, 0, 1)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 1, Vector3F(0, 0, 1)));
    ASSERT_EQ(1.0, shape_func->evaluate_func(0, 2, Vector3F(0, 0, 1)));
}

TEST_F(ShapeFunctionTest, EvalAtNodeCube) {
    FEMeshPtr mesh = load_fem_mesh("cube.msh");
    ShapeFuncPtr shape_func =
        ShapeFunction::create(mesh, "linear");
    ASSERT_EQ(1.0, shape_func->evaluate_func(0, 0, Vector4F(1, 0, 0, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 1, Vector4F(1, 0, 0, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 2, Vector4F(1, 0, 0, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 3, Vector4F(1, 0, 0, 0)));

    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 0, Vector4F(0, 1, 0, 0)));
    ASSERT_EQ(1.0, shape_func->evaluate_func(0, 1, Vector4F(0, 1, 0, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 2, Vector4F(0, 1, 0, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 3, Vector4F(0, 1, 0, 0)));

    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 0, Vector4F(0, 0, 1, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 1, Vector4F(0, 0, 1, 0)));
    ASSERT_EQ(1.0, shape_func->evaluate_func(0, 2, Vector4F(0, 0, 1, 0)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 3, Vector4F(0, 0, 1, 0)));

    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 0, Vector4F(0, 0, 0, 1)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 1, Vector4F(0, 0, 0, 1)));
    ASSERT_EQ(0.0, shape_func->evaluate_func(0, 2, Vector4F(0, 0, 0, 1)));
    ASSERT_EQ(1.0, shape_func->evaluate_func(0, 3, Vector4F(0, 0, 0, 1)));
}

/**
 * For linear shape function over triangle, it has constant grad.
 * The magnitude of grad equals the reciprocal of the triangle height.
 * This is what we are checking here.
 */
TEST_F(ShapeFunctionTest, TriangleGradient) {
    FEMeshPtr mesh = load_fem_mesh("square_2D.obj");
    ShapeFuncPtr shape_func =
        ShapeFunction::create(mesh, "linear");
    Vector3F coord(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0);
    const size_t num_element = mesh->getNbrElements();
    for (size_t i=0; i<num_element; i++) {
        Float height_0 = compute_triangle_height(mesh, i, 0);
        Float height_1 = compute_triangle_height(mesh, i, 1);
        Float height_2 = compute_triangle_height(mesh, i, 2);

        VectorF grad_0 = shape_func->evaluate_grad(i, 0, coord);
        VectorF grad_1 = shape_func->evaluate_grad(i, 1, coord);
        VectorF grad_2 = shape_func->evaluate_grad(i, 2, coord);

        ASSERT_FLOAT_EQ(1.0 / height_0, grad_0.norm());
        ASSERT_FLOAT_EQ(1.0 / height_1, grad_1.norm());
        ASSERT_FLOAT_EQ(1.0 / height_2, grad_2.norm());
    }
}

/**
 * For linear shape function over tetrahedron, it has constant grad.
 * The magnitude of grad equals the reciprocal of the tetrahedron height.
 * This is what we are checking here.
 */
TEST_F(ShapeFunctionTest, TetrahedronGradient) {
    FEMeshPtr mesh = load_fem_mesh("cube.msh");
    ShapeFuncPtr shape_func =
        ShapeFunction::create(mesh, "linear");
    Vector4F coord(1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0);
    const size_t num_element = mesh->getNbrElements();
    for (size_t i=0; i<num_element; i++) {
        Float height_0 = compute_tetrahedron_height(mesh, i, 0);
        Float height_1 = compute_tetrahedron_height(mesh, i, 1);
        Float height_2 = compute_tetrahedron_height(mesh, i, 2);
        Float height_3 = compute_tetrahedron_height(mesh, i, 3);

        VectorF grad_0 = shape_func->evaluate_grad(i, 0, coord);
        VectorF grad_1 = shape_func->evaluate_grad(i, 1, coord);
        VectorF grad_2 = shape_func->evaluate_grad(i, 2, coord);
        VectorF grad_3 = shape_func->evaluate_grad(i, 3, coord);

        ASSERT_FLOAT_EQ(1.0 / height_0, grad_0.norm());
        ASSERT_FLOAT_EQ(1.0 / height_1, grad_1.norm());
        ASSERT_FLOAT_EQ(1.0 / height_2, grad_2.norm());
        ASSERT_FLOAT_EQ(1.0 / height_3, grad_3.norm());
    }
}

