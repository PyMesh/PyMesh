/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <iostream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Environment.h>

#include <Wires/WireNetwork/WireNetwork.h>

#include <WireTest.h>

class WireNetworkTest : public WireTest {
    protected:
        void ASSERT_NONZERO_BBOX(const WireNetwork& network) {
            const size_t dim = network.get_dim();
            const MatrixFr& vertices = network.get_vertices();
            VectorF bbox_min = vertices.colwise().minCoeff();
            VectorF bbox_max = vertices.colwise().maxCoeff();
            ASSERT_EQ(dim, bbox_min.size());
            ASSERT_EQ(dim, bbox_max.size());

            ASSERT_GT((bbox_max - bbox_min).norm(), 0.0);
        }

        void ASSERT_VALID_EDGES(const WireNetwork& network) {
            const size_t num_vertices = network.get_num_vertices();
            const MatrixIr& edges = network.get_edges();
            ASSERT_TRUE((edges.array() >= 0).all());
            ASSERT_TRUE((edges.array() < num_vertices).all());
        }
};

TEST_F(WireNetworkTest, Brick5) {
    std::string wire_file = m_data_dir + "brick5.wire";
    WireNetwork network(wire_file);
    ASSERT_EQ(3, network.get_dim());
    ASSERT_EQ(20, network.get_num_vertices());
    ASSERT_EQ(30, network.get_num_edges());
    ASSERT_NONZERO_BBOX(network);
    ASSERT_VALID_EDGES(network);
}

TEST_F(WireNetworkTest, Box) {
    std::string wire_file = m_data_dir + "box.wire";
    WireNetwork network(wire_file);
    ASSERT_EQ(2, network.get_dim());
    ASSERT_EQ(12, network.get_num_vertices());
    ASSERT_EQ(12, network.get_num_edges());
    ASSERT_NONZERO_BBOX(network);
    ASSERT_VALID_EDGES(network);
}

TEST_F(WireNetworkTest, Attributes) {
    std::string wire_file = m_data_dir + "cube.wire";
    WireNetwork network(wire_file);

    const size_t num_vertices = network.get_num_vertices();
    const size_t num_edges = network.get_num_edges();

    network.add_attribute("test", true);
    network.set_attribute("test", MatrixFr::Ones(num_vertices, 3));
    ASSERT_TRUE(network.has_attribute("test"));
    ASSERT_TRUE(network.is_vertex_attribute("test"));
    ASSERT_FLOAT_EQ(1.0, network.get_attribute("test").minCoeff());
    ASSERT_FLOAT_EQ(1.0, network.get_attribute("test").maxCoeff());

    network.add_attribute("test2", false);
    network.set_attribute("test2", MatrixFr::Zero(num_edges, 3));
    ASSERT_TRUE(network.has_attribute("test2"));
    ASSERT_FALSE(network.is_vertex_attribute("test2"));
    ASSERT_FLOAT_EQ(0.0, network.get_attribute("test2").minCoeff());
    ASSERT_FLOAT_EQ(0.0, network.get_attribute("test2").maxCoeff());
}

TEST_F(WireNetworkTest, Scaling) {
    std::string wire_file = m_data_dir + "cube.wire";
    WireNetwork network(wire_file);

    MatrixFr vertices = network.get_vertices();
    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();
    VectorF bbox_size = bbox_max - bbox_min;

    Vector3F factors(1.0, 2.0, 3.0);
    network.scale(factors);

    MatrixFr scaled_vertices = network.get_vertices();
    VectorF scaled_bbox_min = scaled_vertices.colwise().minCoeff();
    VectorF scaled_bbox_max = scaled_vertices.colwise().maxCoeff();
    VectorF scaled_bbox_size = scaled_bbox_max - scaled_bbox_min;

    ASSERT_VECTOR_EQ(factors, scaled_bbox_size.cwiseQuotient(bbox_size));
}

TEST_F(WireNetworkTest, Offset) {
    std::string wire_file = m_data_dir + "cube.wire";
    WireNetwork network(wire_file);

    MatrixFr vertices = network.get_vertices();
    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();
    VectorF center = 0.5 * (bbox_min + bbox_max); 

    Vector3F offset(1, 1, 1);
    network.translate(offset);
    MatrixFr offseted_vertices = network.get_vertices();
    VectorF offseted_bbox_min = offseted_vertices.colwise().minCoeff();
    VectorF offseted_bbox_max = offseted_vertices.colwise().maxCoeff();
    VectorF offseted_center = 0.5 * (offseted_bbox_min + offseted_bbox_max); 

    ASSERT_VECTOR_EQ(offset, offseted_center - center);
}

TEST_F(WireNetworkTest, ScaleFit) {
    std::string wire_file = m_data_dir + "cube.wire";
    WireNetwork network(wire_file);

    Vector3F bbox_min(-1, -2, -3);
    Vector3F bbox_max( 1,  2,  3);

    network.scale_fit(bbox_min, bbox_max);

    ASSERT_FLOAT_EQ(0.0, (bbox_min - network.get_bbox_min()).norm());
    ASSERT_FLOAT_EQ(0.0, (bbox_max - network.get_bbox_max()).norm());
}

TEST_F(WireNetworkTest, IO) {
    std::string wire_file = m_data_dir + "cube.wire";
    WireNetwork network(wire_file);

    std::string tmp_wire_file = "tmp.wire";
    network.write_to_file(tmp_wire_file);
    WireNetwork tmp_network(tmp_wire_file);

    ASSERT_FLOAT_EQ(0.0, (network.get_vertices() - tmp_network.get_vertices()).norm());
    ASSERT_FLOAT_EQ(0.0, (network.get_edges() - tmp_network.get_edges()).norm());
}
