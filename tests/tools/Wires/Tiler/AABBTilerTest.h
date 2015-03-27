/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <iostream>

#include <WireTest.h>
#include <Wires/Tiler/AABBTiler.h>
#include <Misc/HashGrid.h>

class AABBTilerTest : public WireTest {
    protected:
        void ASSERT_VALID_EDGES(const WireNetwork& network) {
            const size_t num_vertices = network.get_num_vertices();
            const MatrixIr& edges = network.get_edges();
            ASSERT_TRUE((edges.array() >= 0).all());
            ASSERT_TRUE((edges.array() < num_vertices).all());
        }

        void ASSERT_BBOX_SIZE(const WireNetwork& network, const VectorF& bbox_size) {
            const MatrixFr& vertices = network.get_vertices();
            ASSERT_EQ(bbox_size.size(), vertices.cols());
            VectorF bbox_min = vertices.colwise().minCoeff();
            VectorF bbox_max = vertices.colwise().maxCoeff();
            ASSERT_FLOAT_EQ(0.0, (bbox_size - (bbox_max - bbox_min)).norm());
        }

        VectorF get_ori_point(const VectorF& p, const VectorF& base_point, const VectorF& bbox_size) {
            const size_t dim = p.size();
            VectorF result(dim);
            for (size_t i=0; i<dim; i++) {
                result[i] = fmod(p[i] - base_point[i], bbox_size[i])
                    - bbox_size[i] * 0.5;
            }
            return result;
        }

        void check_vertex_attribute_propagation(
                const std::string& attr_name,
                const WireNetwork& network,
                const WireNetwork& tiled_network) {
            const size_t dim = network.get_dim();

            ASSERT_TRUE(network.has_attribute(attr_name));
            ASSERT_TRUE(network.is_vertex_attribute(attr_name));
            ASSERT_TRUE(tiled_network.has_attribute(attr_name));
            ASSERT_TRUE(tiled_network.is_vertex_attribute(attr_name));

            const MatrixFr& attributes = network.get_attribute(attr_name);
            const MatrixFr& tiled_attributes = tiled_network.get_attribute(attr_name);
            ASSERT_EQ(network.get_num_vertices(), attributes.rows());
            ASSERT_EQ(tiled_network.get_num_vertices(), tiled_attributes.rows());

            const MatrixFr& vertices = network.get_vertices();
            const MatrixFr& tiled_vertices = tiled_network.get_vertices();

            const size_t num_vertices = vertices.rows();
            const size_t num_tiled_vertices = tiled_vertices.rows();

            const VectorF bbox_min = vertices.colwise().minCoeff();
            const VectorF bbox_max = vertices.colwise().maxCoeff();
            const VectorF bbox_size = bbox_max - bbox_min;

            const VectorF tiled_bbox_min = tiled_vertices.colwise().minCoeff();

            HashGrid::Ptr grid = HashGrid::create(1e-6, dim);
            for (size_t i=0; i<num_vertices; i++) {
                const VectorF& v = vertices.row(i);
                grid->insert(i, v);
            }

            for (size_t i=0; i<num_tiled_vertices; i++) {
                const VectorF& v = tiled_vertices.row(i);
                VectorF ori_v = get_ori_point(v, tiled_bbox_min, bbox_size);
                VectorI ori_indices = grid->get_items_near_point(ori_v);
                ASSERT_EQ(1, ori_indices.size());
                size_t j = ori_indices[0];
                const VectorF& attr = attributes.row(j);
                const VectorF& tiled_attr = tiled_attributes.row(i);
                ASSERT_FLOAT_EQ(0.0, (attr - tiled_attr).norm());
            }
        }

        void check_edge_attribute_propagation(const std::string& attr_name,
                const WireNetwork& network, const WireNetwork& tiled_network) {
            const size_t dim = network.get_dim();

            ASSERT_TRUE(network.has_attribute(attr_name));
            ASSERT_FALSE(network.is_vertex_attribute(attr_name));
            ASSERT_TRUE(tiled_network.has_attribute(attr_name));
            ASSERT_FALSE(tiled_network.is_vertex_attribute(attr_name));

            const MatrixFr& attributes = network.get_attribute(attr_name);
            const MatrixFr& tiled_attributes = tiled_network.get_attribute(attr_name);
            ASSERT_EQ(network.get_num_edges(), attributes.rows());
            ASSERT_EQ(tiled_network.get_num_edges(), tiled_attributes.rows());

            const MatrixFr& vertices = network.get_vertices();
            const MatrixFr& tiled_vertices = tiled_network.get_vertices();
            const MatrixIr& edges = network.get_edges();
            const MatrixIr& tiled_edges = tiled_network.get_edges();

            const size_t num_edges = edges.rows();
            const size_t num_tiled_edges = tiled_edges.rows();

            const VectorF bbox_min = vertices.colwise().minCoeff();
            const VectorF bbox_max = vertices.colwise().maxCoeff();
            const VectorF bbox_size = bbox_max - bbox_min;

            const VectorF tiled_bbox_min = tiled_vertices.colwise().minCoeff();

            HashGrid::Ptr grid = HashGrid::create(1e-6, dim);
            for (size_t i=0; i<num_edges; i++) {
                const VectorI& edge = edges.row(i);
                VectorF mid_edge_p = 0.5 * (
                        vertices.row(edge[0]) + vertices.row(edge[1]));
                grid->insert(i, mid_edge_p);
            }

            for (size_t i=0; i<num_tiled_edges; i++) {
                const VectorI& edge = tiled_edges.row(i);
                VectorF mid_edge_p = 0.5 * (
                        tiled_vertices.row(edge[0]) +
                        tiled_vertices.row(edge[1]));
                mid_edge_p = get_ori_point(mid_edge_p, tiled_bbox_min, bbox_size);
                VectorI ori_indices = grid->get_items_near_point(mid_edge_p);
                ASSERT_EQ(1, ori_indices.size());
                size_t j = ori_indices[0];
                const VectorF& attr = attributes.row(j);
                const VectorF& tiled_attr = tiled_attributes.row(i);
                ASSERT_FLOAT_EQ(0.0, (attr - tiled_attr).norm());
            }
        }

        void run_periodic_vertex_index_check(const std::string& wire_file) {
            WireNetwork::Ptr wire_network = load_wire_shared(wire_file);
            wire_network->center_at_origin();
            wire_network->add_attribute("vertex_periodic_index");

            const MatrixFr& vertices = wire_network->get_vertices();
            VectorF bbox_min = vertices.colwise().minCoeff() * 3;
            VectorF bbox_max = vertices.colwise().maxCoeff() * 3;
            VectorI repetitions = VectorI::Ones(wire_network->get_dim())*3;

            AABBTiler tiler(wire_network, bbox_min, bbox_max, repetitions);
            WireNetwork::Ptr tiled_network = tiler.tile();

            check_vertex_attribute_propagation("vertex_periodic_index", *wire_network, *tiled_network);
        }

        void run_periodic_edge_index_check(const std::string& wire_file) {
            WireNetwork::Ptr wire_network = load_wire_shared(wire_file);
            wire_network->center_at_origin();
            wire_network->add_attribute("edge_periodic_index");

            const MatrixFr& vertices = wire_network->get_vertices();
            VectorF bbox_min = vertices.colwise().minCoeff() * 3;
            VectorF bbox_max = vertices.colwise().maxCoeff() * 3;
            VectorI repetitions = VectorI::Ones(wire_network->get_dim())*3;

            AABBTiler tiler(wire_network, bbox_min, bbox_max, repetitions);
            WireNetwork::Ptr tiled_network = tiler.tile();

            check_edge_attribute_propagation("edge_periodic_index", *wire_network, *tiled_network);
        }

        void run_min_angle_check(const std::string& wire_file) {
            WireNetwork::Ptr wire_network = load_wire_shared(wire_file);
            wire_network->compute_connectivity();
            wire_network->center_at_origin();

            VectorF bbox_min = wire_network->get_bbox_min() * 3;
            VectorF bbox_max = wire_network->get_bbox_max() * 3;
            VectorI repetitions = VectorI::Ones(wire_network->get_dim())*3;

            AABBTiler tiler(wire_network, bbox_min, bbox_max, repetitions);
            WireNetwork::Ptr tiled_network = tiler.tile();

            wire_network->add_attribute("vertex_min_angle");
            const MatrixFr& min_angles = wire_network->get_attribute("vertex_min_angle");
            ASSERT_LT(1e-6, min_angles.minCoeff());

            tiled_network->compute_connectivity();
            tiled_network->add_attribute("vertex_min_angle");
            const MatrixFr& tiled_min_angles = tiled_network->get_attribute("vertex_min_angle");
            ASSERT_LT(1e-6, tiled_min_angles.minCoeff());
        }
};

TEST_F(AABBTilerTest, cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    VectorF bbox_min = VectorF::Zero(3);
    VectorF bbox_max = VectorF::Ones(3)*2;
    VectorI repetitions = VectorI::Ones(3)*2;

    AABBTiler tiler(wire_network, bbox_min, bbox_max, repetitions);
    WireNetwork::Ptr tiled_network = tiler.tile();

    ASSERT_EQ(27, tiled_network->get_num_vertices());
    ASSERT_EQ(54, tiled_network->get_num_edges());
    ASSERT_VALID_EDGES(*tiled_network);
    ASSERT_BBOX_SIZE(*tiled_network, bbox_max - bbox_min);
}

TEST_F(AABBTilerTest, square) {
    WireNetwork::Ptr wire_network = load_wire_shared("square.wire");
    VectorF bbox_min = VectorF::Zero(2);
    VectorF bbox_max = VectorF::Ones(2)*2;
    VectorI repetitions = VectorI::Ones(2)*2;

    AABBTiler tiler(wire_network, bbox_min, bbox_max, repetitions);
    WireNetwork::Ptr tiled_network = tiler.tile();

    ASSERT_EQ(9, tiled_network->get_num_vertices());
    ASSERT_EQ(12, tiled_network->get_num_edges());
    ASSERT_VALID_EDGES(*tiled_network);
    ASSERT_BBOX_SIZE(*tiled_network, bbox_max - bbox_min);
}

TEST_F(AABBTilerTest, VertexAttributes) {
    run_periodic_vertex_index_check("square.wire");
    run_periodic_vertex_index_check("cube.wire");
    run_periodic_vertex_index_check("box.wire");
    run_periodic_vertex_index_check("brick5.wire");
    run_periodic_vertex_index_check("truncated_octahedron_s1.wire");
}

TEST_F(AABBTilerTest, EdgeAttributes) {
    run_periodic_edge_index_check("square.wire");
    run_periodic_edge_index_check("cube.wire");
    run_periodic_edge_index_check("brick5.wire");
    run_periodic_edge_index_check("box.wire");
    run_periodic_edge_index_check("truncated_octahedron_s1.wire");
}

TEST_F(AABBTilerTest, postive_min_angles) {
    run_min_angle_check("truncated_octahedron_s1.wire");
}
