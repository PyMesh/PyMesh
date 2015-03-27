/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Attributes/WireVertexSupportAttribute.h>
#include <WireTest.h>

class WireVertexSupportTest : public WireTest {
};

TEST_F(WireVertexSupportTest, cube) {
    WireNetwork network = load_wire("cube.wire");
    network.compute_connectivity();

    network.add_attribute("vertex_support_X");
    const MatrixFr& values = network.get_attribute("vertex_support_X");

    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_FLOAT_EQ(1.0, values.minCoeff());
}

TEST_F(WireVertexSupportTest, brick5) {
    WireNetwork network = load_wire("brick5.wire");
    network.compute_connectivity();

    network.add_attribute("vertex_support_Z");
    const MatrixFr& values = network.get_attribute("vertex_support_Z");

    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_FLOAT_EQ(1.0, values.minCoeff());
}

TEST_F(WireVertexSupportTest, brick5_diagonal) {
    WireNetwork network = load_wire("brick5.wire");
    network.compute_connectivity();

    Vector3F print_dir(1, 1, 1);
    WireVertexSupportAttribute attr(print_dir);
    attr.compute(network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(WireAttribute::VERTEX, attr.get_attribute_type());
    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(1.0, values.maxCoeff());
}
