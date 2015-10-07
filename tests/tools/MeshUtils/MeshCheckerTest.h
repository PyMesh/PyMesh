/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <MeshUtils/MeshChecker.h>
#include <TestBase.h>

class MeshCheckerTest : public TestBase {
    protected:
        MeshChecker create(Mesh::Ptr mesh) {
            return MeshChecker(
                    MatrixUtils::reshape<MatrixFr>(
                        mesh->get_vertices(),
                        mesh->get_num_vertices(),
                        mesh->get_dim() ),
                    MatrixUtils::reshape<MatrixIr>(
                        mesh->get_faces(),
                        mesh->get_num_faces(),
                        mesh->get_vertex_per_face()),
                    MatrixUtils::reshape<MatrixIr>(
                        mesh->get_voxels(),
                        mesh->get_num_voxels(),
                        mesh->get_vertex_per_voxel())
                    );
        }
};

TEST_F(MeshCheckerTest, cube) {
    Mesh::Ptr mesh = load_mesh("cube.obj");
    MeshChecker checker = create(mesh);
    ASSERT_TRUE(checker.is_closed());
    ASSERT_TRUE(checker.is_edge_manifold());
    ASSERT_TRUE(checker.is_vertex_manifold());
    ASSERT_EQ(2, checker.get_euler_characteristic());
    ASSERT_EQ(1, checker.get_num_connected_surface_components());
    ASSERT_EQ(0, checker.get_genus());
    ASSERT_EQ(0, checker.get_num_boundary_edges());
    ASSERT_EQ(0, checker.get_num_boundary_loops());
    ASSERT_TRUE(checker.is_oriented());
}

TEST_F(MeshCheckerTest, square) {
    Mesh::Ptr mesh = load_mesh("square_2D.obj");
    MeshChecker checker = create(mesh);
    ASSERT_FALSE(checker.is_closed());
    ASSERT_TRUE(checker.is_edge_manifold());
    ASSERT_TRUE(checker.is_vertex_manifold());
    ASSERT_EQ(1, checker.get_euler_characteristic());
    ASSERT_EQ(1, checker.get_num_connected_surface_components());
    ASSERT_EQ(0, checker.get_genus());
    ASSERT_EQ(4, checker.get_num_boundary_edges());
    ASSERT_EQ(1, checker.get_num_boundary_loops());
    ASSERT_TRUE(checker.is_oriented());
}

TEST_F(MeshCheckerTest, quad) {
    Mesh::Ptr mesh = load_mesh("quad.obj");
    MeshChecker checker = create(mesh);
    ASSERT_FALSE(checker.is_closed());
    ASSERT_TRUE(checker.is_edge_manifold());
    ASSERT_TRUE(checker.is_vertex_manifold());
    ASSERT_EQ(1, checker.get_euler_characteristic());
    ASSERT_EQ(1, checker.get_num_connected_surface_components());
    ASSERT_EQ(0, checker.get_genus());
    ASSERT_EQ(4, checker.get_num_boundary_edges());
    ASSERT_EQ(1, checker.get_num_boundary_loops());
    ASSERT_TRUE(checker.is_oriented());
}

TEST_F(MeshCheckerTest, duplicated_face_count) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;

    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             0, 2, 1;

    MatrixIr voxels(0, 4);

    MeshChecker checker(vertices, faces, voxels);
    ASSERT_EQ(1, checker.get_num_duplicated_faces());
    ASSERT_TRUE(checker.is_oriented());
}

TEST_F(MeshCheckerTest, odd_adj_faces) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0;
    MatrixIr faces(5, 3);
    faces << 0, 2, 1,
             0, 3, 2,
             0, 3, 1,
             1, 2, 3,
             1, 2, 3;

    MatrixIr voxels(0, 4);

    MeshChecker checker(vertices, faces, voxels);
    ASSERT_EQ(1, checker.get_num_duplicated_faces());
    ASSERT_TRUE(checker.has_edge_with_odd_adj_faces());
    ASSERT_FALSE(checker.is_oriented());
}

TEST_F(MeshCheckerTest, hex) {
    Mesh::Ptr mesh = load_mesh("hex.msh");
    MeshChecker checker = create(mesh);
    ASSERT_TRUE(checker.is_closed());
    ASSERT_TRUE(checker.is_edge_manifold());
    ASSERT_TRUE(checker.is_vertex_manifold());
    ASSERT_EQ(2, checker.get_euler_characteristic());
    ASSERT_EQ(1, checker.get_num_connected_surface_components());
    ASSERT_EQ(1, checker.get_num_connected_volume_components());
    ASSERT_EQ(0, checker.get_genus());
    ASSERT_EQ(0, checker.get_num_boundary_edges());
    ASSERT_EQ(0, checker.get_num_boundary_loops());
    ASSERT_TRUE(checker.is_oriented());
}

