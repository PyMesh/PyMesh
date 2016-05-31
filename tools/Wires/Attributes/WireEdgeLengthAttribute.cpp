/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireEdgeLengthAttribute.h"
#include <Wires/WireNetwork/WireNetwork.h>

using namespace PyMesh;

void WireEdgeLengthAttribute::compute(const WireNetwork& network) {
    const size_t num_edges = network.get_num_edges();
    const MatrixFr& vertices = network.get_vertices();
    const MatrixIr& edges = network.get_edges();

    m_values.resize(num_edges, 1);
    for (size_t i=0; i<num_edges; i++) {
        const auto& edge = edges.row(i);
        m_values(i,0) = (vertices.row(edge[0]) - vertices.row(edge[1])).norm();
    }
}
