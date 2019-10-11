/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <TestBase.h>
#include <CGAL/StraightSkeleton.h>

class StraightSkeletonTest : public TestBase {
};

TEST_F(StraightSkeletonTest, triangle) {
    MatrixFr vertices(3, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                0.0, 1.0;
    MatrixIr edges(3, 2);
    edges << 0, 1,
             1, 2,
             2, 0;

    StraightSkeleton engine;
    engine.run(vertices, edges);

    const auto& skeleton_vertices = engine.get_skeleton_vertices();
    const auto& skeleton_edges = engine.get_skeleton_edges();

    ASSERT_LT(skeleton_edges.maxCoeff(), skeleton_vertices.rows());

    ASSERT_EQ(4, skeleton_vertices.rows());
    ASSERT_EQ(3, skeleton_edges.rows());
}

TEST_F(StraightSkeletonTest, square) {
    MatrixFr vertices(4, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                1.0, 1.0,
                0.0, 1.0;

    MatrixIr edges(4, 2);
    edges << 0, 1,
             1, 2,
             2, 3,
             3, 0;

    StraightSkeleton engine;
    engine.run(vertices, edges);

    const auto& skeleton_vertices = engine.get_skeleton_vertices();
    const auto& skeleton_edges = engine.get_skeleton_edges();

    ASSERT_LT(skeleton_edges.maxCoeff(), skeleton_vertices.rows());

    ASSERT_EQ(5, skeleton_vertices.rows());
    ASSERT_EQ(4, skeleton_edges.rows());
}

TEST_F(StraightSkeletonTest, rectangle) {
    MatrixFr vertices(4, 2);
    vertices << 0.0, 0.0,
                2.0, 0.0,
                2.0, 1.0,
                0.0, 1.0;

    MatrixIr edges(4, 2);
    edges << 0, 1,
             1, 2,
             2, 3,
             3, 0;

    StraightSkeleton engine;
    engine.run(vertices, edges);

    const auto& skeleton_vertices = engine.get_skeleton_vertices();
    const auto& skeleton_edges = engine.get_skeleton_edges();

    ASSERT_LT(skeleton_edges.maxCoeff(), skeleton_vertices.rows());

    ASSERT_EQ(6, skeleton_vertices.rows());
    ASSERT_EQ(5, skeleton_edges.rows());
}

#endif
