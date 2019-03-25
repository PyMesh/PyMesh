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

TEST_F(NonmanifoldTest, no_cut) {
    Mesh::Ptr mesh = load_mesh("cube.obj");
    Mesh::Ptr out_mesh = ManifoldCheck::cut_to_manifold(mesh);
    ASSERT_EQ(mesh->get_num_vertices(), out_mesh->get_num_vertices());

    const auto faces = MatrixUtils::reshape<MatrixIr>(
            out_mesh->get_faces(),
            out_mesh->get_num_faces(),
            out_mesh->get_vertex_per_face());
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(faces);
    ASSERT_TRUE((is_v_manifold.array() == 1).all());
    ASSERT_TRUE((is_e_manifold.array() == 1).all());
}

TEST_F(NonmanifoldTest, cut_nonmanifold_edge) {
    MatrixFr vertices(5, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 2.0, 0.0,
                0.0, 3.0, 0.0;
    MatrixIr faces(3, 3);
    faces << 0, 1, 2,
             0, 1, 3,
             0, 1, 4;

    Mesh::Ptr mesh = load_data(vertices, faces);
    Mesh::Ptr out_mesh = ManifoldCheck::cut_to_manifold(mesh);
    ASSERT_EQ(9, out_mesh->get_num_vertices());
    ASSERT_EQ(3, out_mesh->get_num_faces());

    const auto out_faces = MatrixUtils::reshape<MatrixIr>(
            out_mesh->get_faces(),
            out_mesh->get_num_faces(),
            out_mesh->get_vertex_per_face());
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(out_faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(out_faces);
    ASSERT_TRUE((is_v_manifold.array() == 1).all());
    ASSERT_TRUE((is_e_manifold.array() == 1).all());
}

TEST_F(NonmanifoldTest, cut_nonmanifold_vertex) {
    MatrixFr vertices(5, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 2.0, 0.0,
                0.0, 3.0, 0.0;
    MatrixIr faces(3, 3);
    faces << 0, 1, 2,
             0, 1, 3,
             0, 1, 4;

    Mesh::Ptr mesh = load_data(vertices, faces);
    Mesh::Ptr out_mesh = ManifoldCheck::cut_to_manifold(mesh);
    ASSERT_EQ(9, out_mesh->get_num_vertices());
    ASSERT_EQ(3, out_mesh->get_num_faces());

    const auto out_faces = MatrixUtils::reshape<MatrixIr>(
            out_mesh->get_faces(),
            out_mesh->get_num_faces(),
            out_mesh->get_vertex_per_face());
    const auto is_v_manifold = ManifoldCheck::is_vertex_manifold(out_faces);
    const auto is_e_manifold = ManifoldCheck::is_edge_manifold(out_faces);
    ASSERT_TRUE((is_v_manifold.array() == 1).all());
    ASSERT_TRUE((is_e_manifold.array() == 1).all());
}

