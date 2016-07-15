/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <MinkowskiSum.h>
#include <MeshUtils/MeshChecker.h>

class MinkowskiSumTest : public TestBase {
};

TEST_F(MinkowskiSumTest, Simple) {
    MeshPtr mesh = load_mesh("cube.obj");

    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    MatrixFr path(2, 3);
    path << 0, 0, 0,
            1, 0, 0;

    MinkowskiSum ms(vertices, faces);
    ms.run(path);

    VectorF old_bbox_min = vertices.colwise().minCoeff();
    VectorF old_bbox_max = vertices.colwise().maxCoeff();
    VectorF new_bbox_min = ms.get_vertices().colwise().minCoeff();
    VectorF new_bbox_max = ms.get_vertices().colwise().maxCoeff();

    ASSERT_FLOAT_EQ(old_bbox_max[0] + 1, new_bbox_max[0]);
    ASSERT_FLOAT_EQ(old_bbox_max[1], new_bbox_max[1]);
    ASSERT_FLOAT_EQ(old_bbox_max[2], new_bbox_max[2]);

    ASSERT_MATRIX_EQ(old_bbox_min, new_bbox_min);
}

TEST_F(MinkowskiSumTest, SmallOffset) {
    const Float eps = 1e-12;
    MeshPtr mesh = load_mesh("tet.obj");

    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    MatrixFr path(2, 3);
    path << 0, 0, 0,
            eps, eps, eps;

    MinkowskiSum ms(vertices, faces);
    ms.run(path);

    VectorF old_bbox_min = vertices.colwise().minCoeff();
    VectorF old_bbox_max = vertices.colwise().maxCoeff();
    VectorF new_bbox_min = ms.get_vertices().colwise().minCoeff();
    VectorF new_bbox_max = ms.get_vertices().colwise().maxCoeff();

    ASSERT_FLOAT_EQ(old_bbox_max[0] + eps, new_bbox_max[0]);
    ASSERT_FLOAT_EQ(old_bbox_max[1] + eps, new_bbox_max[1]);
    ASSERT_FLOAT_EQ(old_bbox_max[2] + eps, new_bbox_max[2]);

    ASSERT_MATRIX_EQ(old_bbox_min, new_bbox_min);
}

TEST_F(MinkowskiSumTest, Identity) {
    MeshPtr mesh = load_mesh("tet.obj");

    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    MatrixFr path(2, 3);
    path << 0, 0, 0,
            0, 0, 0;

    MinkowskiSum ms(vertices, faces);
    ms.run(path);

    VectorF old_bbox_min = vertices.colwise().minCoeff();
    VectorF old_bbox_max = vertices.colwise().maxCoeff();
    VectorF new_bbox_min = ms.get_vertices().colwise().minCoeff();
    VectorF new_bbox_max = ms.get_vertices().colwise().maxCoeff();

    ASSERT_MATRIX_EQ(old_bbox_min, new_bbox_min);
    ASSERT_MATRIX_EQ(old_bbox_max, new_bbox_max);
}

TEST_F(MinkowskiSumTest, Chain) {
    MeshPtr mesh = load_mesh("tet.obj");
    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    MatrixFr path(4, 3);
    path << 0, 0, 0,
            1, 0, 0,
            1, 1, 0,
            0, 1, 0;

    MinkowskiSum ms(vertices, faces);
    ms.run(path);

    MatrixFr out_vertices = ms.get_vertices();
    MatrixIr out_faces = ms.get_faces();
    MatrixIr out_voxels;
    MeshChecker checker(out_vertices, out_faces, out_voxels);

    ASSERT_TRUE(checker.is_closed());
    ASSERT_TRUE(checker.is_oriented());
    ASSERT_EQ(1, checker.get_num_connected_components());
}
