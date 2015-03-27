/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <ConvexHull/CGAL/CGALConvexHull2D.h>
#include <Core/EigenTypedef.h>
#include <ConvexHullEngineTest.h>

class CGALConvexHull2DTest : public ConvexHullEngineTest {
};

TEST_F(CGALConvexHull2DTest, 2D_square) {
    MatrixFr pts(5, 2);
    pts << 0.0, 0.0,
           1.0, 0.0,
           0.0, 1.0,
           1.0, 1.0,
           0.5, 0.5;

    CGALConvexHull2D engine;
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

TEST_F(CGALConvexHull2DTest, 2D_square_2) {
    MatrixFr pts(6, 2);
    pts << 0.0, 0.0,
           1.0, 0.0,
           0.0, 1.0,
           1.0, 1.0,
           0.5, 0.5,
           0.5, 1.1;

    CGALConvexHull2D engine;
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
