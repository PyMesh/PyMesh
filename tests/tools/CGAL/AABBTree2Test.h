/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <TestBase.h>
#include <CGAL/AABBTree2.h>

#include <Mesh.h>
#include <MeshFactory.h>

class AABBTree2Test : public TestBase {
};

TEST_F(AABBTree2Test, triangle) {
    Matrix2Fr points(3, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              0.0, 1.0;
    Matrix2Ir segments(3, 2);
    segments << 0, 1,
                1, 2,
                2, 0;
    AABBTree2 tree(points, segments);

    Matrix2Fr queries(7, 2);
    queries << 0.0, 0.0,
               1.0, 0.0,
               0.0, 1.0,
               0.5, 0.0,
               0.0, 0.5,
               0.5, 0.5,
               1.0, 1.0;

    VectorF distances;
    VectorI segment_indices;
    Matrix2Fr closest_points;
    tree.look_up_with_closest_points(
            queries, distances, segment_indices, closest_points);

    ASSERT_FLOAT_EQ(0.0, distances[0]);
    ASSERT_FLOAT_EQ(0.0, distances[1]);
    ASSERT_FLOAT_EQ(0.0, distances[2]);

    ASSERT_FLOAT_EQ(0.0, distances[3]);
    ASSERT_FLOAT_EQ(0.0, distances[4]);
    ASSERT_FLOAT_EQ(0.0, distances[5]);

    ASSERT_FLOAT_EQ(0.5, distances[6]);
}

#endif
