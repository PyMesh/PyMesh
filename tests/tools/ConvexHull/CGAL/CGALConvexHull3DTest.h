/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <ConvexHull/CGAL/CGALConvexHull3D.h>
#include <Core/EigenTypedef.h>
#include <ConvexHullEngineTest.h>

class CGALConvexHull3DTest : public ConvexHullEngineTest {
};

TEST_F(CGALConvexHull3DTest, simple_cube) {
    MatrixFr pts(8, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1.0,
           1.0, 0.0, 1.0,
           1.0, 1.0, 1.0,
           0.0, 1.0, 1.0;

    CGALConvexHull3D engine;
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

TEST_F(CGALConvexHull3DTest, interior_points) {
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

    CGALConvexHull3D engine;
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

TEST_F(CGALConvexHull3DTest, near_boundary_points) {
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

    CGALConvexHull3D engine;
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

TEST_F(CGALConvexHull3DTest, near_boundary_points_2) {
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

    CGALConvexHull3D engine;
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

TEST_F(CGALConvexHull3DTest, exterior_points) {
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

    CGALConvexHull3D engine;
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

TEST_F(CGALConvexHull3DTest, near_degenerated_pts) {
    MatrixFr pts(9, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 0.0,
           1.0, 1.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 1e-12,
           1.0, 0.0, 1e-12,
           1.0, 1.0, 1e-12,
           0.0, 1.0, 1e-12,
           0.5, 0.5, 1e-13;

    CGALConvexHull3D engine;
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
