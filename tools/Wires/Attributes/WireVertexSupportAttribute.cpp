/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireVertexSupportAttribute.h"

#include <queue>
#include <Wires/WireNetwork/WireNetwork.h>

using namespace PyMesh;

namespace WireVertexSupportAttributeHelper {
    const Float EPS = 1e-3;
    const Float SUPPORTED = 1.0;
    const Float NOT_SUPPORTED = 0.0;
}
using namespace WireVertexSupportAttributeHelper;

void WireVertexSupportAttribute::compute(const WireNetwork& network) {
    if (network.get_dim()  != 3) {
        throw NotImplementedError("Vertex support is only available to 3D wires");
    }

    if (!network.with_connectivity()) {
        throw RuntimeError("Vertex support attribute requires wire connectivity");
    }

    const size_t dim = network.get_dim();
    const size_t num_vertices = network.get_num_vertices();

    m_values = MatrixFr::Zero(num_vertices, 1);
    mark_directly_supported_vertices(network, m_values);
    propagate_supports(network, m_values);
}

/**
 * Directly supported vertices are vertices on bbox faces that we support
 * directly by connecting it to the base of the built platform.
 */
void WireVertexSupportAttribute::mark_directly_supported_vertices(
        const WireNetwork& network, MatrixFr& markers) {
    const size_t dim = network.get_dim();
    assert(dim == m_print_dir.size());
    VectorF bbox_min = network.get_bbox_min();
    VectorF bbox_max = network.get_bbox_max();

    const size_t num_vertices = network.get_num_vertices();
    const MatrixFr& vertices = network.get_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = vertices.row(i);
        bool is_supported = false;
        for (size_t j=0; j<dim; j++) {
            if (m_print_dir[j] > 0 && fabs(v[j] - bbox_min[j]) < EPS) {
                is_supported = true;
                break;
            }
            if (m_print_dir[j] < 0 && fabs(v[j] - bbox_max[j]) < EPS) {
                is_supported = true;
                break;
            }
        }
        if (is_supported) {
            markers(i, 0) = SUPPORTED;
        }
    }
}

void WireVertexSupportAttribute::propagate_supports(
        const WireNetwork& network, MatrixFr& markers) {
    std::queue<size_t> Q;
    const size_t num_vertices = network.get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        if (markers(i, 0) == SUPPORTED) {
            Q.push(i);
        }
    }

    const MatrixFr& vertices = network.get_vertices();
    while (!Q.empty()) {
        size_t curr_idx = Q.front();
        const VectorF& curr_v = vertices.row(curr_idx);
        Q.pop();

        VectorI neighbors = network.get_vertex_neighbors(curr_idx);
        const size_t num_neighbors = neighbors.size();
        for (size_t i=0; i<num_neighbors; i++) {
            size_t next_idx = neighbors[i];
            if (markers(next_idx, 0) == SUPPORTED) continue;

            const VectorF& next_v = vertices.row(next_idx);
            VectorF dir = next_v - curr_v;
            Float proj = dir.dot(m_print_dir);
            if (proj >= -EPS) {
                markers(next_idx, 0) = SUPPORTED;
                Q.push(next_idx);
            }
        }
    }
}
