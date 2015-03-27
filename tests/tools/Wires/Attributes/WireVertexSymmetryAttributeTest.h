/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Attributes/WireVertexSymmetryAttribute.h>
#include <WireTest.h>

class WireVertexSymmetryAttributeTest : public WireTest {
};

TEST_F(WireVertexSymmetryAttributeTest, cube) {
    WireNetwork::Ptr network = load_wire_shared("cube.wire");
    WireVertexSymmetryAttribute attr;
    attr.compute(*network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());
}

TEST_F(WireVertexSymmetryAttributeTest, brick5) {
    WireNetwork::Ptr network = load_wire_shared("brick5.wire");
    WireVertexSymmetryAttribute attr;
    attr.compute(*network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(6.0, values.maxCoeff());

    ASSERT_FLOAT_EQ(
            values.block(0,0,8,1).minCoeff(),
            values.block(0,0,8,1).maxCoeff());

    ASSERT_FLOAT_EQ(values(8, 0),  values(10, 0));
    ASSERT_FLOAT_EQ(values(9, 0),  values(13, 0));
    ASSERT_FLOAT_EQ(values(11, 0), values(12, 0));
    ASSERT_FLOAT_EQ(values(14, 0), values(15, 0));
    ASSERT_FLOAT_EQ(values(16, 0), values(17, 0));
    ASSERT_FLOAT_EQ(values(18, 0), values(19, 0));
}

TEST_F(WireVertexSymmetryAttributeTest, square) {
    WireNetwork::Ptr network = load_wire_shared("square.wire");
    WireVertexSymmetryAttribute attr;
    attr.compute(*network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());
}

TEST_F(WireVertexSymmetryAttributeTest, box) {
    WireNetwork::Ptr network = load_wire_shared("box.wire");
    WireVertexSymmetryAttribute attr;
    attr.compute(*network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network->get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(4.0, values.maxCoeff());

    ASSERT_FLOAT_EQ(
            values.block(0,0,4,1).minCoeff(),
            values.block(0,0,4,1).maxCoeff());
    ASSERT_FLOAT_EQ(values(8, 0), values(10, 0));
    ASSERT_FLOAT_EQ(values(4, 0), values(6, 0));
    ASSERT_FLOAT_EQ(values(5, 0), values(7, 0));
    ASSERT_FLOAT_EQ(values(9, 0), values(11, 0));
}
