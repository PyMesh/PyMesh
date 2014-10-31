#pragma once

#include <cmath>
#include <string>
#include <iostream>

#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

#include <MeshUtils/DuplicatedVertexRemoval.h>

class DuplicatedVertexRemovalTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(MeshFactory()
                    .load_file(mesh_file)
                    .create());
        }

        void assert_vector_eq(const VectorF& v1, const VectorF& v2) {
            VectorF diff = v1 - v2;
            ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
            ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
        }

        void assert_matrix_eq(const MatrixFr& m1, const MatrixFr& m2) {
            MatrixFr diff = m1 - m2;
            ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
            ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
        }

        void assert_matrix_eq(const MatrixIr& m1, const MatrixIr& m2) {
            MatrixIr diff = m1 - m2;
            ASSERT_EQ(0, diff.minCoeff());
            ASSERT_EQ(0, diff.maxCoeff());
        }

        void assert_valid_index_map(
                const MatrixFr& vertices_old,
                const MatrixFr& vertices_new,
                const VectorI& index_map, Float tol=1e-6) {
            ASSERT_EQ(index_map.size(), vertices_old.rows());
            const size_t num_old_vertices = vertices_old.rows();
            for (size_t i=0; i<num_old_vertices; i++) {
                size_t new_i = index_map[i];
                const VectorF& old_v = vertices_old.row(i);
                const VectorF& new_v = vertices_new.row(new_i);
                ASSERT_NEAR(0.0, (new_v-old_v).norm(), tol);
            }
        }

    protected:
        std::string m_data_dir;
};

TEST_F(DuplicatedVertexRemovalTest, 2D) {
    const Float tol = 1e-3;
    MeshPtr mesh = load_mesh("square_2D.obj");
    const size_t dim = 2;
    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    MatrixFr vertices = Eigen::Map<MatrixFr>(
            mesh->get_vertices().data(), num_vertices, dim);
    MatrixIr faces = Eigen::Map<MatrixIr>(
            mesh->get_faces().data(), num_faces, vertex_per_face);

    // Move vertex 0 to coincide with vertex 1
    vertices.row(0) = vertices.row(1);

    MatrixFr expected_vertices(num_vertices-1, dim);
    MatrixIr expected_faces(num_faces, vertex_per_face);

    for (size_t i=1; i<num_vertices; i++) {
        expected_vertices.row(i-1) = vertices.row(i);
    }
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            expected_faces.coeffRef(i,j) = faces.coeff(i,j) - 1;
            if (expected_faces.coeff(i,j) < 0)
                expected_faces.coeffRef(i,j) = 0;
        }
    }

    DuplicatedVertexRemoval remover(vertices, faces);
    remover.run(tol);

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  index_map = remover.get_index_map();

    assert_matrix_eq(expected_vertices, result_vertices);
    assert_matrix_eq(expected_faces, result_faces);
    assert_valid_index_map(vertices, result_vertices, index_map, tol);

    ASSERT_EQ(vertices.rows() - 1, result_vertices.rows());
    ASSERT_EQ(faces.rows(), result_faces.rows());
    ASSERT_LT(result_faces.maxCoeff(), result_vertices.rows());
}

TEST_F(DuplicatedVertexRemovalTest, 3D) {
    const Float tol = 1e-3;
    MeshPtr mesh = load_mesh("cube.msh");
    const size_t dim = 3;
    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t vertex_per_face = mesh->get_vertex_per_face();

    MatrixFr vertices = Eigen::Map<MatrixFr>(
            mesh->get_vertices().data(), num_vertices, dim);
    MatrixIr faces = Eigen::Map<MatrixIr>(
            mesh->get_faces().data(), num_faces, vertex_per_face);

    // Move vertex 0 to coincide with vertex 1
    vertices.row(0) = vertices.row(1);

    MatrixFr expected_vertices(num_vertices-1, dim);
    MatrixIr expected_faces(num_faces, vertex_per_face);

    for (size_t i=1; i<num_vertices; i++) {
        expected_vertices.row(i-1) = vertices.row(i);
    }
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            expected_faces.coeffRef(i,j) = faces.coeff(i,j) - 1;
            if (expected_faces.coeff(i,j) < 0)
                expected_faces.coeffRef(i,j) = 0;
        }
    }

    DuplicatedVertexRemoval remover(vertices, faces);
    remover.run(tol);

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  index_map = remover.get_index_map();

    assert_matrix_eq(expected_vertices, result_vertices);
    assert_matrix_eq(expected_faces, result_faces);
    assert_valid_index_map(vertices, result_vertices, index_map, tol);

    ASSERT_EQ(vertices.rows() - 1, result_vertices.rows());
    ASSERT_EQ(faces.rows(), result_faces.rows());
    ASSERT_LT(result_faces.maxCoeff(), result_vertices.rows());
}

TEST_F(DuplicatedVertexRemovalTest, very_close_points) {
    const Float tol = 1.0 - 1e-6;
    MatrixF vertices(3, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                0.5, 0.5 * sqrt(3);
    MatrixI faces(1, 3);
    faces << 0, 1, 2;

    DuplicatedVertexRemoval remover(vertices, faces);
    remover.run(tol);

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  index_map = remover.get_index_map();

    ASSERT_EQ(3, result_vertices.rows());
}

TEST_F(DuplicatedVertexRemovalTest, not_close_enough_points) {
    const Float tol = 1.0 + 1e-6;
    MatrixF vertices(3, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                0.5, 0.5 * sqrt(3);
    MatrixI faces(1, 3);
    faces << 0, 1, 2;

    DuplicatedVertexRemoval remover(vertices, faces);
    remover.run(tol);

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  index_map = remover.get_index_map();

    ASSERT_EQ(1, result_vertices.rows());
}
