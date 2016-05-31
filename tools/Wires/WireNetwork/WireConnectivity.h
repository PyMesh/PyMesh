/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>

namespace PyMesh {

class WireNetwork;

class WireConnectivity {
    public:
        WireConnectivity() : m_wire_network(NULL) {}

        /**
         * Copy constructor is needed because m_wire_network should not be
         * copied.
         */
        WireConnectivity(const WireConnectivity& other) {
            m_vertex_adjacencies = other.m_vertex_adjacencies;
            m_vertex_adjacency_index = other.m_vertex_adjacency_index;
        }

        /**
         * operator=() is needed because m_wire_network should not be
         * copied.
         */
        WireConnectivity& operator=(const WireConnectivity& other) {
            m_vertex_adjacencies = other.m_vertex_adjacencies;
            m_vertex_adjacency_index = other.m_vertex_adjacency_index;
            return *this;
        }

    public:
        void set_wire_network(const WireNetwork* wire_network) {
            m_wire_network = wire_network;
        }
        const WireNetwork* get_wire_network() const { return m_wire_network; }

        void compute();
        void reset();
        bool is_available() const { return m_vertex_adjacencies.size() > 0; }

    public:
        VectorI get_vertex_neighbors(size_t i) const;

    private:
        const WireNetwork* m_wire_network;

        VectorI m_vertex_adjacencies;
        VectorI m_vertex_adjacency_index;
};

}
