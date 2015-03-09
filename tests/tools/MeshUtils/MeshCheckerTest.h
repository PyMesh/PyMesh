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
                        mesh->get_vertex_per_face()));
        }
};

TEST_F(MeshCheckerTest, cube) {
    Mesh::Ptr mesh = load_mesh("cube.obj");
    MeshChecker checker = create(mesh);
    ASSERT_TRUE(checker.is_closed());
    ASSERT_TRUE(checker.is_edge_manifold());
    ASSERT_TRUE(checker.is_vertex_manifold());
    ASSERT_EQ(2, checker.get_euler_characteristic());
    ASSERT_EQ(1, checker.get_num_connected_components());
    ASSERT_EQ(0, checker.get_genus());
    ASSERT_EQ(0, checker.get_num_boundary_edges());
    ASSERT_EQ(0, checker.get_num_boundary_loops());
}

TEST_F(MeshCheckerTest, square) {
    Mesh::Ptr mesh = load_mesh("square_2D.obj");
    MeshChecker checker = create(mesh);
    ASSERT_FALSE(checker.is_closed());
    ASSERT_TRUE(checker.is_edge_manifold());
    ASSERT_TRUE(checker.is_vertex_manifold());
    ASSERT_EQ(1, checker.get_euler_characteristic());
    ASSERT_EQ(1, checker.get_num_connected_components());
    ASSERT_EQ(0, checker.get_genus());
    ASSERT_EQ(4, checker.get_num_boundary_edges());
    ASSERT_EQ(1, checker.get_num_boundary_loops());
}

TEST_F(MeshCheckerTest, quad) {
    Mesh::Ptr mesh = load_mesh("quad.obj");
    MeshChecker checker = create(mesh);
    ASSERT_FALSE(checker.is_closed());
    ASSERT_TRUE(checker.is_edge_manifold());
    ASSERT_THROW(checker.is_vertex_manifold(), NotImplementedError);
    ASSERT_EQ(1, checker.get_euler_characteristic());
    ASSERT_EQ(1, checker.get_num_connected_components());
    ASSERT_EQ(0, checker.get_genus());
    ASSERT_EQ(4, checker.get_num_boundary_edges());
    ASSERT_EQ(1, checker.get_num_boundary_loops());
}

TEST_F(MeshCheckerTest, duplicated_face_count) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;

    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             0, 2, 1;

    MeshChecker checker(vertices, faces);
    ASSERT_EQ(1, checker.get_num_duplicated_faces());
}

