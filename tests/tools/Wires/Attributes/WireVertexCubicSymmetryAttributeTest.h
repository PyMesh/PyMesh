/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Attributes/WireVertexCubicSymmetryAttribute.h>
#include <WireTest.h>

class WireVertexCubicSymmetryAttributeTest : public WireTest {
};

TEST_F(WireVertexCubicSymmetryAttributeTest, cube) {
    WireNetwork::Ptr network = load_wire_shared("cube.wire");
    network->add_attribute("vertex_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute("vertex_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());
}

TEST_F(WireVertexCubicSymmetryAttributeTest, brick5) {
    WireNetwork::Ptr network = load_wire_shared("brick5.wire");
    network->add_attribute("vertex_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute("vertex_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(2.0, values.maxCoeff());
}

TEST_F(WireVertexCubicSymmetryAttributeTest, pattern0050) {
    WireNetwork::Ptr network = load_wire_shared("pattern0050.wire");
    network->add_attribute("vertex_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute("vertex_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(2.0, values.maxCoeff());
}

TEST_F(WireVertexCubicSymmetryAttributeTest, square) {
    WireNetwork::Ptr network = load_wire_shared("square.wire");
    network->add_attribute("vertex_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute("vertex_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());
}

TEST_F(WireVertexCubicSymmetryAttributeTest, box) {
    WireNetwork::Ptr network = load_wire_shared("box.wire");
    network->add_attribute("vertex_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute("vertex_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(2.0, values.maxCoeff());
}
