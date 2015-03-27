/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <WireTest.h>
#include <Wires/Parameters/SymmetryOrbits.h>
#include <Wires/WireNetwork/WireNetwork.h>

class SymmetryOrbitsTest : public WireTest {
    protected:
        SymmetryOrbits load_orbits(const std::string& orbit_file) {
            return SymmetryOrbits(m_data_dir + orbit_file);
        }

        void ASSERT_ORBITS_ARE_VALID(WireNetwork::Ptr wire_network, SymmetryOrbits& orbits) {
            const size_t num_vertices = wire_network->get_num_vertices();
            const size_t num_edges = wire_network->get_num_edges();
            const size_t num_vertex_orbits = orbits.get_num_vertex_orbits();
            const size_t num_edge_orbits = orbits.get_num_edge_orbits();

            VectorI vertex_orbit_idx = VectorI::Zero(num_vertices);
            VectorI edge_orbit_idx = VectorI::Zero(num_edges);

            for (size_t i=0; i<num_vertex_orbits; i++) {
                const VectorI& orbit = orbits.get_vertex_orbit(i);
                const size_t size = orbit.size();
                for (size_t j=0; j<size; j++) {
                    ASSERT_LE(0, orbit[j]);
                    ASSERT_GT(num_vertices, orbit[j]);
                    vertex_orbit_idx[orbit[j]] = i+1;
                }
            }
            ASSERT_EQ(1, vertex_orbit_idx.minCoeff());
            ASSERT_EQ(num_vertex_orbits, vertex_orbit_idx.maxCoeff());

            for (size_t i=0; i<num_edge_orbits; i++) {
                const VectorI& orbit = orbits.get_edge_orbit(i);
                const size_t size = orbit.size();
                for (size_t j=0; j<size; j++) {
                    ASSERT_LE(0, orbit[j]);
                    ASSERT_GT(num_edges, orbit[j]);
                    edge_orbit_idx[orbit[j]] = i+1;
                }
            }
            ASSERT_EQ(1, edge_orbit_idx.minCoeff());
            ASSERT_EQ(num_edge_orbits, edge_orbit_idx.maxCoeff());
        }
};

TEST_F(SymmetryOrbitsTest, cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    SymmetryOrbits orbits = load_orbits("cube.orbit");

    ASSERT_EQ(1, orbits.get_num_vertex_orbits());
    ASSERT_EQ(3, orbits.get_num_edge_orbits());
    ASSERT_ORBITS_ARE_VALID(wire_network, orbits);
}

TEST_F(SymmetryOrbitsTest, box) {
    WireNetwork::Ptr wire_network = load_wire_shared("box.wire");
    SymmetryOrbits orbits = load_orbits("box.orbit");

    ASSERT_EQ(5, orbits.get_num_vertex_orbits());
    ASSERT_EQ(4, orbits.get_num_edge_orbits());
    ASSERT_ORBITS_ARE_VALID(wire_network, orbits);
}
