/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/WireNetwork/WireConnectivity.h>
#include <WireTest.h>

class WireConnectivityTest : public WireTest {
    protected:
        void ASSERT_IS_ADJACENT(const WireConnectivity& conn, size_t i, size_t j) {
            VectorI adj_i = conn.get_vertex_neighbors(i);
            VectorI adj_j = conn.get_vertex_neighbors(j);

            ASSERT_TRUE((adj_i.array() == j).any());
            ASSERT_TRUE((adj_j.array() == i).any());
        }

        void ASSERT_CONNECTIVITY_IS_VALID(
                const WireNetwork& network, const WireConnectivity& conn) {
            const MatrixIr& edges = network.get_edges();
            const size_t num_edges = network.get_num_edges();
            for (size_t i=0; i<num_edges; i++) {
                VectorI edge = edges.row(i);
                ASSERT_IS_ADJACENT(conn, edge[0], edge[1]);
            }
        }
};

TEST_F(WireConnectivityTest, cube) {
    WireNetwork network = load_wire("cube.wire");
    WireConnectivity conn;
    conn.set_wire_network(&network);
    conn.compute();
    ASSERT_CONNECTIVITY_IS_VALID(network, conn);
}

TEST_F(WireConnectivityTest, Brick5) {
    WireNetwork network = load_wire("brick5.wire");
    WireConnectivity conn;
    conn.set_wire_network(&network);
    conn.compute();
    ASSERT_CONNECTIVITY_IS_VALID(network, conn);
}
