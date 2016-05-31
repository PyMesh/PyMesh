/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireConnectivity.h"
#include "WireNetwork.h"

#include <list>
#include <vector>

#include <Core/Exception.h>

using namespace PyMesh;

void WireConnectivity::compute() {
    const size_t num_vertices = m_wire_network->get_num_vertices();
    const size_t num_edges = m_wire_network->get_num_edges();
    std::vector<std::list<size_t> > adjacencies(num_vertices);

    const MatrixIr& edges = m_wire_network->get_edges();
    for (size_t i=0; i<num_edges; i++) {
        const VectorI& edge = edges.row(i);
        adjacencies[edge[0]].push_back(edge[1]);
        adjacencies[edge[1]].push_back(edge[0]);
    }

    size_t count=0;
    m_vertex_adjacency_index.resize(num_vertices+1);
    m_vertex_adjacency_index[0] = 0;
    std::list<size_t> all_adjacencies;
    for (auto adj : adjacencies) {
        m_vertex_adjacency_index[count+1] =
            m_vertex_adjacency_index[count] + adj.size();
        all_adjacencies.splice(all_adjacencies.end(), adj);
        count++;
    }

    m_vertex_adjacencies.resize(all_adjacencies.size());
    std::copy(all_adjacencies.begin(), all_adjacencies.end(),
            m_vertex_adjacencies.data());
}

void WireConnectivity::reset() {
    m_vertex_adjacencies.resize(0);
    m_vertex_adjacency_index.resize(0);
}

VectorI WireConnectivity::get_vertex_neighbors(size_t i) const {
    if (m_vertex_adjacencies.size() == 0) {
        throw RuntimeError("Vertex adjacencies is not computed.");
    }

    size_t begin = m_vertex_adjacency_index[i];
    size_t end   = m_vertex_adjacency_index[i+1];
    return m_vertex_adjacencies.segment(begin, end-begin);
}
