/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "SubdivisionTest.h"

class LoopSubdivisionTest : public SubdivisionTest {
    protected:
        SubDivPtr create_subdivision() {
            return Subdivision::create("loop");
        }
};

TEST_F(LoopSubdivisionTest, creation) {
    SubDivPtr sub = create_subdivision();
}

TEST_F(LoopSubdivisionTest, square) {
    MeshPtr square = load_mesh("square_2D.obj");
    MatrixFr vertices = extract_vertices(square);
    MatrixIr faces = extract_faces(square);
    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(2, faces.rows());

    SubDivPtr sub = create_subdivision();
    sub->subdivide(vertices, faces, 1);

    MatrixFr sub_vertices = sub->get_vertices();
    MatrixIr sub_faces = sub->get_faces();

    ASSERT_EQ(8, sub_faces.rows());
    ASSERT_EQ(9, sub_vertices.rows());
    assert_equal_centroid(vertices, sub_vertices);
}

TEST_F(LoopSubdivisionTest, cube) {
    MeshPtr cube = load_mesh("cube.obj");
    MatrixFr vertices = extract_vertices(cube);
    MatrixIr faces = extract_faces(cube);
    ASSERT_EQ(8, vertices.rows());
    ASSERT_EQ(12, faces.rows());

    SubDivPtr sub = create_subdivision();
    sub->subdivide(vertices, faces, 1);

    MatrixFr sub_vertices = sub->get_vertices();
    MatrixIr sub_faces = sub->get_faces();

    ASSERT_EQ(26, sub_vertices.rows());
    ASSERT_EQ(48, sub_faces.rows());
    assert_equal_centroid(vertices, sub_vertices);
}

TEST_F(LoopSubdivisionTest, face_index) {
    MeshPtr square = load_mesh("square_2D.obj");
    MatrixFr vertices = extract_vertices(square);
    MatrixIr faces = extract_faces(square);

    SubDivPtr sub = create_subdivision();
    sub->subdivide(vertices, faces, 1);

    MatrixIr sub_faces = sub->get_faces();
    VectorI face_indices = sub->get_face_indices();

    ASSERT_EQ(sub_faces.rows(), face_indices.size());
    ASSERT_EQ(0, face_indices.minCoeff());
    ASSERT_EQ(faces.rows()-1, face_indices.maxCoeff());
}
