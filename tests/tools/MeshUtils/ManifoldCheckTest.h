/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <MeshUtils/ManifoldCheck.h>
#include <TestBase.h>

class NonmanifoldTest : public TestBase {
};

TEST_F(NonmanifoldTest, cube) {
    Mesh::Ptr mesh = load_mesh("cube.obj");
    const auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_TRUE((is_v_manifold.array() == 1).all());
    ASSERT_TRUE((is_e_manifold.array() == 1).all());
}

TEST_F(NonmanifoldTest, butterfly) {
    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             0, 4, 3;
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_EQ(0, is_v_manifold[0]);
    ASSERT_EQ(1, is_v_manifold.segment<4>(1).minCoeff());
    ASSERT_TRUE((is_e_manifold.array() == 1).all());
}

TEST_F(NonmanifoldTest, nonmanifold_edge) {
    MatrixIr faces(3, 3);
    faces << 0, 1, 2,
             0, 1, 3,
             0, 1, 4;
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_EQ(0, is_v_manifold[0]);
    ASSERT_EQ(0, is_v_manifold[1]);
    ASSERT_EQ(1, is_v_manifold[2]);
    ASSERT_EQ(1, is_v_manifold[3]);
    ASSERT_EQ(1, is_v_manifold[4]);
    ASSERT_TRUE((is_e_manifold.block(0, 0, 3, 1).array() == 0).all());
    ASSERT_TRUE((is_e_manifold.block(0, 1, 3, 2).array() == 1).all());
}

TEST_F(NonmanifoldTest, empty) {
    MatrixIr faces(0, 3);
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_EQ(0, is_v_manifold.size());
    ASSERT_EQ(0, is_e_manifold.size());
}

TEST_F(NonmanifoldTest, degenerate_triangles) {
    MatrixIr faces(3, 3);
    faces << 0, 0, 1,
             0, 1, 2,
             0, 1, 3;
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_EQ(4, is_v_manifold.size());
    ASSERT_EQ(0, is_v_manifold[0]);
    ASSERT_EQ(0, is_v_manifold[1]);
    MatrixIr edge_manifold(3, 3);
    edge_manifold << 1, 0, 0,
                     0, 1, 1,
                     0, 1, 1;
    ASSERT_MATRIX_EQ(edge_manifold, is_e_manifold);
}

TEST_F(NonmanifoldTest, quad) {
    Mesh::Ptr mesh = load_mesh("quad.obj");
    const auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_TRUE((is_v_manifold.array() == 1).all());
    ASSERT_TRUE((is_e_manifold.array() == 1).all());
}

TEST_F(NonmanifoldTest, hex) {
    Mesh::Ptr mesh = load_mesh("hex.msh");
    const auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_TRUE((is_v_manifold.array() == 1).all());
    ASSERT_TRUE((is_e_manifold.array() == 1).all());
}
