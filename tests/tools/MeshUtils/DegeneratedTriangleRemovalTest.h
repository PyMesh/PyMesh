/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <string>
#include <iostream>

#include <MeshUtils/DegeneratedTriangleRemoval.h>

#include <TestBase.h>

class DegeneratedTriangleRemovalTest : public TestBase {
    protected:
        void assert_no_degeneracy(
                MatrixFr& vertices,
                MatrixIr& faces) {
            if (faces.rows() == 0) return;
            MatrixIr voxels(0, 0);
            MeshPtr mesh = load_data(vertices, faces, voxels);
            mesh->add_attribute("face_area");
            VectorF areas = mesh->get_attribute("face_area");

            ASSERT_LT(0.0, areas.minCoeff());
        }
};

TEST_F(DegeneratedTriangleRemovalTest, no_degeneracy) {
    MeshPtr mesh = load_mesh("cube.msh");
    const size_t dim = 3;
    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t vertex_per_face = mesh->get_vertex_per_face();

    MatrixFr vertices = Eigen::Map<MatrixFr>(
            mesh->get_vertices().data(), num_vertices, dim);
    MatrixIr faces = Eigen::Map<MatrixIr>(
            mesh->get_faces().data(), num_faces, vertex_per_face);

    DegeneratedTriangleRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  ori_face_indices = remover.get_ori_face_indices();

    ASSERT_MATRIX_EQ(vertices, result_vertices);
    ASSERT_MATRIX_EQ(faces, result_faces);
    assert_no_degeneracy(result_vertices, result_faces);
    ASSERT_EQ(0, ori_face_indices.minCoeff());
    ASSERT_EQ(num_faces-1, ori_face_indices.maxCoeff());
}

TEST_F(DegeneratedTriangleRemovalTest, simple) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.5, 0.5, 0.0;
    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             2, 1, 3;

    DegeneratedTriangleRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  ori_face_indices = remover.get_ori_face_indices();

    ASSERT_EQ(4, result_vertices.rows());
    ASSERT_EQ(2, result_faces.rows());
    assert_no_degeneracy(result_vertices, result_faces);
    ASSERT_EQ(0, ori_face_indices.minCoeff());
    ASSERT_EQ(0, ori_face_indices.maxCoeff());
}

TEST_F(DegeneratedTriangleRemovalTest, simple2) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.5, 0.5, 0.0;
    MatrixIr faces(3, 3);
    faces << 0, 1, 2,
             2, 1, 3,
             2, 1, 3;

    DegeneratedTriangleRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  ori_face_indices = remover.get_ori_face_indices();

    ASSERT_EQ(4, result_vertices.rows());
    ASSERT_EQ(2, result_faces.rows());
    assert_no_degeneracy(result_vertices, result_faces);
    ASSERT_EQ(0, ori_face_indices.minCoeff());
    ASSERT_EQ(0, ori_face_indices.maxCoeff());
}

TEST_F(DegeneratedTriangleRemovalTest, zero_edge) {
    MatrixFr vertices(5, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.5, 0.5, 0.0,
                0.5, 0.5, 0.0;
    MatrixIr faces(3, 3);
    faces << 0, 1, 2,
             2, 1, 3,
             2, 1, 4;

    DegeneratedTriangleRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  ori_face_indices = remover.get_ori_face_indices();

    ASSERT_EQ(4, result_vertices.rows());
    ASSERT_EQ(2, result_faces.rows());
    assert_no_degeneracy(result_vertices, result_faces);
    ASSERT_EQ(0, ori_face_indices.minCoeff());
    ASSERT_EQ(0, ori_face_indices.maxCoeff());
}

TEST_F(DegeneratedTriangleRemovalTest, multiple_degeneracies) {
    MatrixFr vertices(6, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                2.0, 0.0, 0.0,
                3.0, 0.0, 0.0,
                4.0, 0.0, 0.0,
                2.0, 1.0, 0.0;
    MatrixIr faces(4, 3);
    faces << 0, 1, 4, // degenerated
             1, 2, 4, // degenerated
             2, 3, 4, // degenerated
             0, 4, 5; // ok

    DegeneratedTriangleRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  ori_face_indices = remover.get_ori_face_indices();

    ASSERT_EQ(6, result_vertices.rows());
    ASSERT_EQ(4, result_faces.rows());
    assert_no_degeneracy(result_vertices, result_faces);
    ASSERT_TRUE((ori_face_indices.array() == 3).all());
}

TEST_F(DegeneratedTriangleRemovalTest, single_degenerated_triangle) {
    // Single degenerated triangle will simply be removed.
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                2.0, 0.0, 0.0;
    MatrixIr faces(1, 3);
    faces << 0, 1, 2;

    DegeneratedTriangleRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  ori_face_indices = remover.get_ori_face_indices();

    ASSERT_EQ(0, result_vertices.rows());
    ASSERT_EQ(0, result_faces.rows());
    assert_no_degeneracy(result_vertices, result_faces);
    ASSERT_EQ(0, ori_face_indices.size());
}

TEST_F(DegeneratedTriangleRemovalTest, all_degenerated_triangles) {
    // If all triangles are degenerated, all is removed.
    MatrixFr vertices(6, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                2.0, 0.0, 0.0,
                3.0, 0.0, 0.0,
                4.0, 0.0, 0.0,
                2.5, 0.0, 0.0;
    MatrixIr faces(4, 3);
    faces << 0, 1, 4,
             1, 2, 4,
             2, 3, 4,
             0, 4, 5;

    DegeneratedTriangleRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI  ori_face_indices = remover.get_ori_face_indices();

    ASSERT_EQ(0, result_vertices.rows());
    ASSERT_EQ(0, result_faces.rows());
    assert_no_degeneracy(result_vertices, result_faces);
    ASSERT_EQ(0, ori_face_indices.size());
}

