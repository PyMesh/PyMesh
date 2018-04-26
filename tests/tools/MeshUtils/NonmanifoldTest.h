/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <MeshUtils/detect_nonmanifold_vertices.h>
#include <TestBase.h>

class NonmanifoldTest : public TestBase {
};

TEST_F(NonmanifoldTest, cube) {
    Mesh::Ptr mesh = load_mesh("cube.obj");
    const auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    const auto is_manifold = detect_nonmanifold_vertices(faces);
    ASSERT_TRUE((is_manifold.array() == 1).all());
}

TEST_F(NonmanifoldTest, butterfly) {
    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             0, 4, 3;
    const auto is_manifold = detect_nonmanifold_vertices(faces);
    ASSERT_EQ(0, is_manifold[0]);
    ASSERT_EQ(1, is_manifold.segment<4>(1).minCoeff());
}

TEST_F(NonmanifoldTest, nonmanifold_edge) {
    MatrixIr faces(3, 3);
    faces << 0, 1, 2,
             0, 1, 3,
             0, 1, 4;
    const auto is_manifold = detect_nonmanifold_vertices(faces);
    ASSERT_EQ(0, is_manifold[0]);
    ASSERT_EQ(0, is_manifold[1]);
    ASSERT_EQ(1, is_manifold[2]);
    ASSERT_EQ(1, is_manifold[3]);
    ASSERT_EQ(1, is_manifold[4]);
}

TEST_F(NonmanifoldTest, empty) {
    MatrixIr faces(0, 3);
    const auto is_manifold = detect_nonmanifold_vertices(faces);
    ASSERT_EQ(0, is_manifold.size());
}

TEST_F(NonmanifoldTest, degenerate_triangles) {
    MatrixIr faces(3, 3);
    faces << 0, 0, 1,
             0, 1, 2,
             0, 1, 3;
    const auto is_manifold = detect_nonmanifold_vertices(faces);
    ASSERT_EQ(4, is_manifold.size());
    ASSERT_EQ(0, is_manifold[0]);
    ASSERT_EQ(0, is_manifold[1]);
}

TEST_F(NonmanifoldTest, quad) {
    Mesh::Ptr mesh = load_mesh("quad.obj");
    const auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    const auto is_manifold = detect_nonmanifold_vertices(faces);
    ASSERT_TRUE((is_manifold.array() == 1).all());
}

TEST_F(NonmanifoldTest, hex) {
    Mesh::Ptr mesh = load_mesh("hex.msh");
    const auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    const auto is_manifold = detect_nonmanifold_vertices(faces);
    ASSERT_TRUE((is_manifold.array() == 1).all());
}
