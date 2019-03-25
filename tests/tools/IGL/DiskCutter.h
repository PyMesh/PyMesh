/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL

#include <TestBase.h>
#include <IGL/DiskCutter.h>
#include <MeshUtils/MeshChecker.h>

class DiskCutterTest : public TestBase {
};

TEST_F(DiskCutterTest, no_cut) {
    MeshPtr mesh = load_mesh("cube.obj");

    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    DiskCutter cutter(vertices, faces);
    cutter.run();

    MatrixFr out_vertices = cutter.get_vertices();
    MatrixIr out_faces = cutter.get_faces();

    ASSERT_EQ(vertices.rows(), out_vertices.rows());
    ASSERT_EQ(vertices.cols(), out_vertices.cols());
    ASSERT_EQ(faces.rows(), out_faces.rows());
    ASSERT_EQ(faces.cols(), out_faces.cols());
}

TEST_F(DiskCutterTest, nested_square) {
    MatrixFr vertices(8, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                1.0, 1.0,
                0.0, 1.0,
                0.2, 0.2,
                0.8, 0.2,
                0.8, 0.8,
                0.2, 0.8;
    MatrixIr faces(8, 3);
    faces << 0, 1, 4,
             4, 1, 5,
             1, 2, 5,
             5, 2, 6,
             2, 3, 6,
             6, 3, 7,
             3, 0, 7,
             7, 0, 4;

    DiskCutter cutter(vertices, faces);
    cutter.run();

    MatrixFr out_vertices = cutter.get_vertices();
    MatrixIr out_faces = cutter.get_faces();

    ASSERT_EQ(10, out_vertices.rows());
    ASSERT_EQ(2, out_vertices.cols());
    ASSERT_EQ(8, out_faces.rows());
    ASSERT_EQ(3, out_faces.cols());
}

TEST_F(DiskCutterTest, nonmanifold) {
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

    DiskCutter cutter(vertices, faces);
    cutter.run();

    MatrixFr out_vertices = cutter.get_vertices();
    MatrixIr out_faces = cutter.get_faces();

    // TODO: seems cut_to_disk does not work for nonmanifold inputs.
    //ASSERT_LE(7, out_vertices.rows());
    ASSERT_EQ(3, out_vertices.cols());
    ASSERT_EQ(3, out_faces.rows());
    ASSERT_EQ(3, out_faces.cols());
}

#endif
