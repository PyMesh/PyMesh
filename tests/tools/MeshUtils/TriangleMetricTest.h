/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <MeshUtils/TriangleMetric.h>
#include <TestBase.h>

class TriangleMetricTest : public TestBase {
    protected:
};

TEST_F(TriangleMetricTest, corner_touch_corner) {
    MatrixFr tri1(3, 3);
    tri1 << 0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0;
    MatrixFr tri2(3, 3);
    tri2 << 0.0, 0.0, 0.0,
           -1.0, 0.0, 0.0,
            0.0,-1.0, 0.0;

    TriangleMetric metric;
    metric.update(tri1, tri2);
    Float dist = metric.getDistance();
    ASSERT_FLOAT_EQ(0.0, dist);
}

TEST_F(TriangleMetricTest, corner_touch_edge) {
    MatrixFr tri1(3, 3);
    tri1 << 0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0;
    MatrixFr tri2(3, 3);
    tri2 << 0.5, 0.0, 0.0,
           -1.0, 0.0, 1.0,
            0.0,-1.0, 1.0;

    TriangleMetric metric;
    metric.update(tri1, tri2);
    Float dist = metric.getDistance();
    ASSERT_FLOAT_EQ(0.0, dist);
}

TEST_F(TriangleMetricTest, corner_touch_face) {
    MatrixFr tri1(3, 3);
    tri1 << 0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0;
    MatrixFr tri2(3, 3);
    tri2 << 0.1, 0.1, 0.0,
           -1.0, 0.0, 1.0,
            0.0,-1.0, 1.0;

    TriangleMetric metric;
    metric.update(tri1, tri2);
    Float dist = metric.getDistance();
    ASSERT_FLOAT_EQ(0.0, dist);
}

TEST_F(TriangleMetricTest, edge_touch_edge) {
    MatrixFr tri1(3, 3);
    tri1 << 0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0;
    MatrixFr tri2(3, 3);
    tri2 << 0.0, 0.5, 2.0,
            0.0, 0.5,-2.0,
           -1.0, 0.0, 0.0;

    TriangleMetric metric;
    metric.update(tri1, tri2);
    Float dist = metric.getDistance();
    ASSERT_FLOAT_EQ(0.0, dist);
}

TEST_F(TriangleMetricTest, DISABLED_edge_touch_face) {
    // The distance returned when two triangles intersects at a line is wrong!!!
    // TODO: update triangle metric class.
    MatrixFr tri1(3, 3);
    tri1 << 0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0;
    MatrixFr tri2(3, 3);
    tri2 << 0.1, 0.5, 2.0,
            0.1, 0.5,-2.0,
           -1.0, 0.0, 0.0;

    TriangleMetric metric;
    metric.setUseLinCanny(false);
    metric.update(tri1, tri2);
    Float dist = metric.getDistance();
    ASSERT_FLOAT_EQ(0.0, dist);
}


