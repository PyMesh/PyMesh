/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireNetwork.h"
#include "WireParser.h"
#include "WireWriter.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

namespace WireNetworkHelper {
    template<typename Derived>
    void filter(const Eigen::MatrixBase<Derived>& data,
            const std::vector<bool>& to_keep,
            Derived& out) {
        const size_t num_data = data.rows();
        const size_t dim = data.cols();
        assert(num_data == to_keep.size());

        size_t num_kept= 0;
        for (size_t i=0; i<num_data; i++) {
            if (to_keep[i]) num_kept++;
        }

        out.resize(num_kept, dim);
        size_t count=0;
        for (size_t i=0; i<num_data; i++) {
            if (to_keep[i]) {
                out.row(count) = data.row(i);
                count++;
            }
        }
    }
}
using namespace WireNetworkHelper;

WireNetwork::Ptr WireNetwork::create(const std::string& wire_file) {
    return Ptr(new WireNetwork(wire_file));
}

WireNetwork::Ptr WireNetwork::create_raw(
        const MatrixFr& vertices, const MatrixIr& edges) {
    return Ptr(new WireNetwork(vertices, edges));
}

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

    update_bbox();
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
    update_bbox();
}

void WireNetwork::center_at_origin() {
    translate(-1 * center());
}

VectorF WireNetwork::center() const {
    return 0.5 * (get_bbox_min() + get_bbox_max());
}

void WireNetwork::filter_vertices(const std::vector<bool>& to_keep) {
    const size_t num_vertices = m_vertices.rows();
    MatrixFr vertices;
    filter(m_vertices, to_keep, vertices);
    m_vertices = vertices;
    const auto& attr_names = m_attributes.get_attribute_names();
    for (const auto& name : attr_names) {
        if (is_vertex_attribute(name)) {
            const MatrixFr& val = m_attributes.get_attribute(name);
            MatrixFr filtered_val;
            filter(val, to_keep, filtered_val);
            m_attributes.set_attribute(*this, name, filtered_val);
        }
    }

    size_t count = 0;
    VectorI index_map = VectorI::Ones(num_vertices) * -1;
    for (size_t i=0; i<num_vertices; i++) {
        if (to_keep[i]) {
            index_map[i] = count;
            count++;
        }
    }

    const size_t num_edges = m_edges.rows();
    std::vector<bool> edge_to_keep(num_edges, false);
    for (size_t i=0; i<num_edges; i++) {
        m_edges(i, 0) = index_map[m_edges(i, 0)];
        m_edges(i, 1) = index_map[m_edges(i, 1)];
        if ((m_edges.row(i).array() >= 0).all()) {
            edge_to_keep[i] = true;
        }
    }
    filter_edges(edge_to_keep);
    assert((m_edges.array() >= 0).all());

    m_connectivity.reset();
    update_bbox();
}

void WireNetwork::filter_edges(const std::vector<bool>& to_keep) {
    MatrixIr edges;
    filter(m_edges, to_keep, edges);
    m_edges = edges;
    const auto& attr_names = m_attributes.get_attribute_names();
    for (const auto& name : attr_names) {
        if (!is_vertex_attribute(name)) {
            const MatrixFr& val = m_attributes.get_attribute(name);
            MatrixFr filtered_val;
            filter(val, to_keep, filtered_val);
            m_attributes.set_attribute(*this, name, filtered_val);
        }
    }
    m_connectivity.reset();
}

void WireNetwork::write_to_file(const std::string& filename) const {
    WireWriter writer;
    writer.write(filename, *this);
}

void WireNetwork::initialize() {
    initialize_connectivity();
    update_bbox();
}

void WireNetwork::initialize_connectivity() {
    m_connectivity.set_wire_network(this);
}

void WireNetwork::update_bbox() {
    m_bbox_min = m_vertices.colwise().minCoeff();
    m_bbox_max = m_vertices.colwise().maxCoeff();
}

bool WireNetwork::is_printable(const std::string& print_dir) {
    if (!with_connectivity()) { compute_connectivity(); }

    std::string attr_name = std::string("vertex_support_") + print_dir;
    if (has_attribute(attr_name)) {
        m_attributes.remove_attribute(attr_name);
    }
    add_attribute(attr_name);
    const MatrixFr& vertex_supported = get_attribute(attr_name);
    return (vertex_supported.array() > 0.5).all();
}

