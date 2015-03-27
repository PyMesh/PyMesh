/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Misc/DistanceComputation.h>
#include <cmath>

using namespace DistanceComputation;

class DistanceComputationTest : public ::testing::Test {
};

TEST_F(DistanceComputationTest, Simple) {
    Vector3F p(0.0, 0.0, 0.0);
    Vector3F c0(1.0, 0.0, 0.0);
    Vector3F c1(1.0, 1.0, 0.0);
    Vector3F c2(-1.0, 0.0, 0.0);
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(c0, c0, c1, c2));
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(c1, c0, c1, c2));
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(c2, c0, c1, c2));
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(p, c0, c1, c2));
}

TEST_F(DistanceComputationTest, Outside) {
    Vector3F p(1.1, 0.0, 0.0);
    Vector3F c0(1.0, 0.0, 0.0);
    Vector3F c1(1.0, 1.0, 0.0);
    Vector3F c2(-1.0, 0.0, 0.0);
    ASSERT_FLOAT_EQ((p-c0).norm(), point_to_triangle(p, c0, c1, c2));
}

TEST_F(DistanceComputationTest, Edge) {
    Vector3F p(1.5, 0.5, 0.0);
    Vector3F c0(1.0, 0.0, 0.0);
    Vector3F c1(1.0, 1.0, 0.0);
    Vector3F c2(-1.0, 0.0, 0.0);
    ASSERT_FLOAT_EQ(0.5, point_to_triangle(p, c0, c1, c2));
}

TEST_F(DistanceComputationTest, Slim) {
    Vector3F c0(-1e-3, 0.0, 0.0);
    Vector3F c1(0.0, -1e-3, 0.0);
    Vector3F c2(1.0, 1.0, 0.0);

    Vector3F o(0.0, 0.0, 0.0);
    Vector3F p(-1e-3, -1e-3, 0.0);
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(o, c0, c1, c2));
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(c0, c0, c1, c2));
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(c1, c0, c1, c2));
    ASSERT_FLOAT_EQ(0.0, point_to_triangle(c2, c0, c1, c2));
    ASSERT_FLOAT_EQ(sqrt(2) * 0.5e-3, point_to_triangle(p, c0, c1, c2));
}
