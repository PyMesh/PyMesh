/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Attributes/WireEdgePeriodicIndexAttribute.h>
#include <WireTest.h>

#include <iostream>
#include <set>

class WireEdgePeriodicIndexAttributeTest : public WireTest {
};

TEST_F(WireEdgePeriodicIndexAttributeTest, cube) {
    WireNetwork network = load_wire("cube.wire");
    WireEdgePeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();
    std::set<int> unique_values(values.data(), values.data() + values.size());

    ASSERT_EQ(network.get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_EQ(3, unique_values.size());
}

TEST_F(WireEdgePeriodicIndexAttributeTest, square) {
    WireNetwork network = load_wire("square.wire");
    WireEdgePeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();
    std::set<int> unique_values(values.data(), values.data() + values.size());

    ASSERT_EQ(network.get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_EQ(2, unique_values.size());
}

TEST_F(WireEdgePeriodicIndexAttributeTest, brick5) {
    WireNetwork network = load_wire("brick5.wire");
    WireEdgePeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();
    std::set<int> unique_values(values.data(), values.data() + values.size());

    ASSERT_EQ(network.get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_EQ(network.get_num_edges(), unique_values.size());
}

TEST_F(WireEdgePeriodicIndexAttributeTest, box) {
    WireNetwork network = load_wire("box.wire");
    WireEdgePeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();
    std::set<int> unique_values(values.data(), values.data() + values.size());

    ASSERT_EQ(network.get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_EQ(network.get_num_edges(), unique_values.size());
}
