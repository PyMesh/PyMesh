/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <string>
#include <iostream>

#include <MeshUtils/DuplicatedVertexRemoval.h>

#include <TestBase.h>

class DuplicatedVertexRemovalTest : public TestBase {
    protected:
        void assert_vector_eq(const VectorF& v1, const VectorF& v2) {
            VectorF diff = v1 - v2;
            ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
            ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
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

    ASSERT_MATRIX_EQ(expected_vertices, result_vertices);
    ASSERT_MATRIX_EQ(expected_faces, result_faces);
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

    ASSERT_MATRIX_EQ(expected_vertices, result_vertices);
    ASSERT_MATRIX_EQ(expected_faces, result_faces);
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

TEST_F(DuplicatedVertexRemovalTest, importance_level) {
    const Float tol = 1e-3;
    MatrixF vertices(3, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                1.0, 1e-4;
    MatrixI faces(1, 3);
    faces << 0, 1, 2;

    Vector3I importance_level(0, 0, 1);
    DuplicatedVertexRemoval remover(vertices, faces);
    remover.set_importance_level(importance_level);
    remover.run(tol);

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  index_map = remover.get_index_map();

    ASSERT_EQ(2, result_vertices.rows());
    ASSERT_FLOAT_EQ(0.0, (result_vertices.row(0) - vertices.row(0)).norm());
    ASSERT_FLOAT_EQ(0.0, (result_vertices.row(1) - vertices.row(2)).norm());
    ASSERT_EQ(0, index_map[0]);
    ASSERT_EQ(1, index_map[1]);
    ASSERT_EQ(1, index_map[1]);
}

TEST_F(DuplicatedVertexRemovalTest, importance_level_2) {
    const Float tol = 1e-3;
    MatrixF vertices(3, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                1.0, 1e-4;
    MatrixI faces(1, 3);
    faces << 0, 1, 2;

    Vector3I importance_level(0, 1, 0);
    DuplicatedVertexRemoval remover(vertices, faces);
    remover.set_importance_level(importance_level);
    remover.run(tol);

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  index_map = remover.get_index_map();

    ASSERT_EQ(2, result_vertices.rows());
    ASSERT_FLOAT_EQ(0.0, (result_vertices.row(0) - vertices.row(0)).norm());
    ASSERT_FLOAT_EQ(0.0, (result_vertices.row(1) - vertices.row(1)).norm());
    ASSERT_EQ(0, index_map[0]);
    ASSERT_EQ(1, index_map[1]);
    ASSERT_EQ(1, index_map[1]);
}
