/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Wires/Attributes/WireEdgeCubicSymmetryAttribute.h>
#include <WireTest.h>

class WireEdgeCubicSymmetryAttributeTest : public WireTest {
    protected:
        void ASSERT_ORBIT_IS_VALID(
                const MatrixFr& vertices,
                const MatrixIr& edges,
                const MatrixFr& labels) {
            const size_t num_edges = edges.rows();
            std::vector<std::vector<VectorF> > edge_orbits(num_edges);
            for (size_t i=0; i<num_edges; i++) {
                const VectorI& e = edges.row(i);
                VectorF edge_vector = (vertices.row(e[0]) - vertices.row(e[1])).cwiseAbs();
                edge_orbits[int(std::round(labels(i)))].push_back(edge_vector);
            }

            for (auto& edge_orbit : edge_orbits) {
                if (edge_orbit.empty()) continue;
                Float edge_len = edge_orbit[0].squaredNorm();
                for (auto& edge : edge_orbit) {
                    Float len = edge.squaredNorm();
                    ASSERT_FLOAT_EQ(edge_len, len);
                }
            }
        }
};

TEST_F(WireEdgeCubicSymmetryAttributeTest, cube) {
    WireNetwork::Ptr network = load_wire_shared("cube.wire");
    network->add_attribute("edge_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute(
            "edge_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());

    ASSERT_ORBIT_IS_VALID(
            network->get_vertices(), network->get_edges(), values);
}

TEST_F(WireEdgeCubicSymmetryAttributeTest, brick5) {
    WireNetwork::Ptr network = load_wire_shared("brick5.wire");
    network->add_attribute("edge_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute(
            "edge_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(1.0, values.maxCoeff());

    ASSERT_ORBIT_IS_VALID(
            network->get_vertices(), network->get_edges(), values);
}

TEST_F(WireEdgeCubicSymmetryAttributeTest, pattern0050) {
    WireNetwork::Ptr network = load_wire_shared("pattern0050.wire");
    network->add_attribute("edge_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute(
            "edge_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(1.0, values.maxCoeff());

    ASSERT_ORBIT_IS_VALID(
            network->get_vertices(), network->get_edges(), values);
}

TEST_F(WireEdgeCubicSymmetryAttributeTest, square) {
    WireNetwork::Ptr network = load_wire_shared("square.wire");
    network->add_attribute("edge_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute(
            "edge_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());

    ASSERT_ORBIT_IS_VALID(
            network->get_vertices(), network->get_edges(), values);
}

TEST_F(WireEdgeCubicSymmetryAttributeTest, box) {
    WireNetwork::Ptr network = load_wire_shared("box.wire");
    network->add_attribute("edge_cubic_symmetry_orbit");
    const MatrixFr& values = network->get_attribute(
            "edge_cubic_symmetry_orbit");

    ASSERT_EQ(network->get_num_edges(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(1.0, values.maxCoeff());

    ASSERT_ORBIT_IS_VALID(
            network->get_vertices(), network->get_edges(), values);
}
