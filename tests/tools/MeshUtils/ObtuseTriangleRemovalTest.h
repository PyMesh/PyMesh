/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <MeshUtils/ObtuseTriangleRemoval.h>
#include <TestBase.h>

class ObtuseTriangleRemovalTest : public TestBase {
    protected:
};

TEST_F(ObtuseTriangleRemovalTest, good_triangles) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.5, 1.0, 0.0,
                0.5,-1.0, 0.0;
    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             0, 3, 1;

    ObtuseTriangleRemoval remover(vertices, faces);
    remover.run(M_PI * 0.75);

    auto result_vertices = remover.get_vertices();
    auto result_faces = remover.get_faces();

    ASSERT_MATRIX_EQ(vertices, result_vertices);
    ASSERT_MATRIX_EQ(faces, result_faces);
}

TEST_F(ObtuseTriangleRemovalTest, single_obtuse) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.5, 1.0, 0.0,
                0.5,-1e-3, 0.0;

    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             0, 3, 1;

    ObtuseTriangleRemoval remover(vertices, faces);
    remover.run(M_PI * 0.75);

    auto result_vertices = remover.get_vertices();
    auto result_faces = remover.get_faces();

    ASSERT_EQ(5, result_vertices.rows());
    ASSERT_EQ(4, result_faces.rows());
}

TEST_F(ObtuseTriangleRemovalTest, double_obtuse) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.25, 1e-3, 0.0,
                0.75,-1e-3, 0.0;

    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             0, 3, 1;
    write_mesh_raw("input.msh", vertices, faces);

    ObtuseTriangleRemoval remover(vertices, faces);
    remover.run(M_PI * 0.75);
    remover.run(M_PI * 0.75);

    auto result_vertices = remover.get_vertices();
    auto result_faces = remover.get_faces();

    write_mesh_raw("debug.msh", result_vertices, result_faces);

    ASSERT_EQ(7, result_vertices.rows());
    ASSERT_EQ(7, result_faces.rows());
}
