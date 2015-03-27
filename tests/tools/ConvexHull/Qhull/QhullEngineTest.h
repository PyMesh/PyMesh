/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <ConvexHull/Qhull/QhullEngine.h>
#include <Core/EigenTypedef.h>
#include <ConvexHullEngineTest.h>

class QhullEngineTest : public ConvexHullEngineTest {
};

TEST_F(QhullEngineTest, simple_cube) {
    MatrixFr pts(8, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1.0,
           1.0, 0.0, 1.0,
           1.0, 1.0, 1.0,
           0.0, 1.0, 1.0;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(8, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_EQ(12, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_SAME_BBOX(pts, vertices);
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}

TEST_F(QhullEngineTest, interior_points) {
    MatrixFr pts(10, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1.0,
           1.0, 0.0, 1.0,
           1.0, 1.0, 1.0,
           0.0, 1.0, 1.0,
           0.5, 0.5, 0.5,
           0.9, 0.1, 0.9;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(8, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_EQ(12, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_SAME_BBOX(pts, vertices);
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}

TEST_F(QhullEngineTest, near_boundary_points) {
    MatrixFr pts(10, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1.0,
           1.0, 0.0, 1.0,
           1.0, 1.0, 1.0,
           0.0, 1.0, 1.0,
           0.5, 1e-6, 1e-6,
           0.9, 1e-6, 0.9;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(8, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_EQ(12, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_SAME_BBOX(pts, vertices);
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}

TEST_F(QhullEngineTest, near_boundary_points_2) {
    MatrixFr pts(9, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1.0,
           1.0, 0.0, 1.0,
           1.0, 1.0, 1.0,
           0.0, 1.0, 1.0,
           0.9, -1e-6, 0.9;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(9, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_EQ(14, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}

TEST_F(QhullEngineTest, exterior_points) {
    MatrixFr pts(9, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1.0,
           1.0, 0.0, 1.0,
           1.0, 1.0, 1.0,
           0.0, 1.0, 1.0,
           0.5, 0.5, 2.0;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(9, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_EQ(14, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}

TEST_F(QhullEngineTest, near_degenerated_pts) {
    MatrixFr pts(9, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1e-6,
           1.0, 0.0, 1e-6,
           1.0, 1.0, 1e-6,
           0.0, 1.0, 1e-6,
           0.5, 0.5, 1e-13;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(8, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_EQ(12, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_SAME_BBOX(pts, vertices);
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}

TEST_F(QhullEngineTest, 2D_square) {
    MatrixFr pts(5, 2);
    pts << 0.0, 0.0,
           1.0, 0.0,
           0.0, 1.0,
           1.0, 1.0,
           0.5, 0.5;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_EQ(4, faces.rows());
    ASSERT_EQ(2, faces.cols());
    ASSERT_SAME_BBOX(pts, vertices);
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}

TEST_F(QhullEngineTest, 2D_square_2) {
    MatrixFr pts(6, 2);
    pts << 0.0, 0.0,
           1.0, 0.0,
           0.0, 1.0,
           1.0, 1.0,
           0.5, 0.5,
           0.5, 1.1;

    QhullEngine engine;
    engine.run(pts);

    MatrixFr vertices = engine.get_vertices();
    MatrixIr faces = engine.get_faces();

    ASSERT_EQ(5, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_EQ(5, faces.rows());
    ASSERT_EQ(2, faces.cols());
    ASSERT_SAME_BBOX(pts, vertices);
    ASSERT_INDEX_MAP_IS_VALID(pts, vertices, engine.get_index_map());
    ASSERT_ORIENTATION_IS_VALID(vertices, faces);
}
