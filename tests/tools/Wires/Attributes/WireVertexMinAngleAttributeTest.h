/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>

#include <Wires/Attributes/WireVertexMinAngleAttribute.h>
#include <WireTest.h>

class WireVertexMinAngleAttributeTest : public WireTest {
};

TEST_F(WireVertexMinAngleAttributeTest, cube) {
    WireNetwork network = load_wire("cube.wire");
    network.compute_connectivity();
    WireVertexMinAngleAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(WireAttribute::VERTEX, attr.get_attribute_type());
    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_FLOAT_EQ(M_PI * 0.5, values.minCoeff());
    ASSERT_FLOAT_EQ(M_PI * 0.5, values.maxCoeff());
}

TEST_F(WireVertexMinAngleAttributeTest, box) {
    WireNetwork network = load_wire("box.wire");
    network.compute_connectivity();
    WireVertexMinAngleAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(WireAttribute::VERTEX, attr.get_attribute_type());
    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_FLOAT_EQ(M_PI * 0.5, values.minCoeff());
    ASSERT_FLOAT_EQ(M_PI, values.maxCoeff());
}
