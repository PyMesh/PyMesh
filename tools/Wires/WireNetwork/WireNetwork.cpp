#include "WireNetwork.h"
#include "WireParser.h"

#include <iostream>
#include <sstream>

#include <Core/Exception.h>

WireNetwork::WireNetwork() : m_dim(0) {
    initialize();
}

WireNetwork::WireNetwork(const std::string& wire_file) {
    WireParser parser;
    parser.parse(wire_file);

    size_t num_vertices = parser.get_num_vertices();
    size_t num_edges = parser.get_num_edges();
    m_dim = parser.get_dim();

    m_vertices.resize(num_vertices, m_dim);
    m_edges.resize(num_edges, 2);

    parser.export_vertices(m_vertices.data());
    parser.export_edges(m_edges.data());

    initialize();
}

WireNetwork::WireNetwork(const MatrixFr& vertices, const MatrixIr& edges)
    : m_vertices(vertices), m_edges(edges)
{
    m_dim = m_vertices.cols();
    initialize();
}

void WireNetwork::scale(const VectorF& factors) {
    if (factors.size() != m_dim) {
        std::stringstream err_msg;
        err_msg << "Scaling factors is of dim ("
            << factors.size() << "), expecting (" << m_dim << ")";
        throw RuntimeError(err_msg.str());
    }

    const size_t num_vertices = get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = m_vertices.row(i);
        m_vertices.row(i) = v.cwiseProduct(factors);
    }
}

void WireNetwork::scale_fit(const VectorF& bbox_min, const VectorF& bbox_max) {
    center_at_origin();

    VectorF bbox_size = bbox_max - bbox_min;
    VectorF bbox_center = 0.5 * (bbox_min + bbox_max);
    VectorF cur_bbox_size = get_bbox_max() - get_bbox_min();

    VectorF scale_factor = bbox_size.cwiseQuotient(cur_bbox_size);
    scale(scale_factor);

    translate(bbox_center);
}

void WireNetwork::translate(const VectorF& offset) {
    if (offset.size() != m_dim) {
        std::stringstream err_msg;
        err_msg << "Offset is of dim ("
            << offset.size() << "), expecting (" << m_dim << ")";
        throw RuntimeError(err_msg.str());
    }

    const size_t num_vertices = get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        m_vertices.row(i) += offset;
    }
}

void WireNetwork::center_at_origin() {
    translate(-1 * center());
}

VectorF WireNetwork::center() const {
    return 0.5 * (get_bbox_min() + get_bbox_max());
}

void WireNetwork::initialize() {
    initialize_connectivity();
}

void WireNetwork::initialize_connectivity() {
    m_connectivity.set_wire_network(this);
}
