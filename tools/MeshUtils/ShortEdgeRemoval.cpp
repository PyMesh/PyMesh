#include "ShortEdgeRemoval.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <set>

#include <Core/Exception.h>

#include "IndexHeap.h"

const size_t ShortEdgeRemoval::UNMAPPED = std::numeric_limits<size_t>::max();

ShortEdgeRemoval::ShortEdgeRemoval(MatrixFr& vertices, MatrixIr& faces) :
    m_vertices(vertices),
    m_faces(faces),
    m_num_collapsed(0),
    m_heap(false) {
        const size_t vertex_per_face = m_faces.cols();
        if (vertex_per_face != 3) {
            throw NotImplementedError("Only triangle faces are supported!");
        }
}

size_t ShortEdgeRemoval::run(Float threshold) {
    init();
    do {
        collapse(threshold);
        update();
    } while (get_num_faces() > 0 && min_edge_length() <= threshold);
    return m_num_collapsed;
}

MatrixFr ShortEdgeRemoval::get_vertices() const {
    const size_t dim = get_dim();
    const size_t num_vertices = get_num_vertices();
    const size_t num_ori_vertices = m_vertices.rows();
    const size_t num_new_vertices = m_new_vertices.size();
    MatrixFr vertices(num_vertices, dim);
    std::copy(m_vertices.data(), m_vertices.data() + m_vertices.size(),
            vertices.data());
    for (size_t i=0; i<num_new_vertices; i++) {
        vertices.row(num_ori_vertices + i) = m_new_vertices[i];
    }
    return vertices;
}

MatrixIr ShortEdgeRemoval::get_faces() const {
    return m_faces;
}

void ShortEdgeRemoval::init() {
    init_vertex_map();
    init_edges();
    init_edge_length_heap();
}

void ShortEdgeRemoval::update() {
    update_faces();
    init_vertex_map();
    init_edges();
    init_edge_length_heap();
}

void ShortEdgeRemoval::init_vertex_map() {
    const size_t num_vertices = get_num_vertices();
    m_vertex_map.resize(num_vertices);
    std::fill(m_vertex_map.begin(), m_vertex_map.end(), UNMAPPED);
}

void ShortEdgeRemoval::init_edges() {
    std::set<Edge> edges;
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t v1_idx = m_faces(i, j);
            size_t v2_idx = m_faces(i, (j+1) % vertex_per_face);
            if (v1_idx != v2_idx)
                edges.insert(Edge(v1_idx, v2_idx));
        }
    }

    const size_t num_edges = edges.size();
    m_edges.resize(num_edges);
    std::copy(edges.begin(), edges.end(), m_edges.begin());
}

void ShortEdgeRemoval::init_edge_length_heap() {
    const size_t num_edges = m_edges.size();
    std::vector<Float> edge_lengths(num_edges);
    for (size_t i=0; i<num_edges; i++) {
        edge_lengths[i] = compute_edge_length(m_edges[i]);
    }
    m_heap.init(edge_lengths);
}

void ShortEdgeRemoval::update_faces() {
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    const size_t num_entries = num_faces * vertex_per_face;

    std::vector<size_t> faces;
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = m_faces.row(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t mapped_idx = m_vertex_map[face[j]];
            if (mapped_idx != UNMAPPED)
                face[j] = m_vertex_map[face[j]];
        }
        if (face[0] == face[1] ||
            face[1] == face[2] ||
            face[2] == face[0])
            continue;
        faces.insert(faces.end(), face.data(), face.data() + vertex_per_face);
    }

    m_faces.resize(faces.size() / vertex_per_face, vertex_per_face);
    std::copy(faces.begin(), faces.end(), m_faces.data());
}

void ShortEdgeRemoval::collapse(Float threshold) {
    while (!m_heap.empty()) {
        size_t edge_idx = m_heap.top();
        Float edge_len = m_heap.top_value();
        m_heap.pop();

        if (edge_len > threshold) break;
        if (!edge_is_valid(edge_idx)) continue;

        collapse_edge(edge_idx);
    }
}

bool ShortEdgeRemoval::edge_is_valid(size_t edge_idx) const {
    const Edge& edge = m_edges[edge_idx];
    size_t v1_idx = edge.get_ori_data()[0];
    size_t v2_idx = edge.get_ori_data()[1];
    return m_vertex_map[v1_idx] == UNMAPPED &&
           m_vertex_map[v2_idx] == UNMAPPED;
}

void ShortEdgeRemoval::collapse_edge(size_t edge_idx) {
    const Edge& e = m_edges[edge_idx];
    const size_t dim = m_vertices.cols();
    const size_t num_ori_vertices = m_vertices.rows();
    const size_t num_new_vertices = m_new_vertices.size();
    const size_t i1 = e.get_ori_data()[0];
    const size_t i2 = e.get_ori_data()[1];
    const VectorF v1 = get_vertex(i1);
    const VectorF v2 = get_vertex(i2);

    VectorF v_mid = 0.5 * (v1 + v2);
    m_new_vertices.push_back(v_mid);

    size_t idx_mid = num_ori_vertices + num_new_vertices;
    m_vertex_map[i1] = idx_mid;
    m_vertex_map[i2] = idx_mid;

    m_num_collapsed++;
}

VectorF ShortEdgeRemoval::get_vertex(size_t i) const {
    const size_t num_ori_vertices = m_vertices.rows();
    if (i<num_ori_vertices) {
        return m_vertices.row(i);
    } else {
        i -= num_ori_vertices;
        assert(i < m_new_vertices.size());
        return m_new_vertices[i];
    }
}

Float ShortEdgeRemoval::min_edge_length() const {
    if (m_heap.size() <= 0)
        throw RuntimeError("Edge heap is empty!");
    return m_heap.top_value();
}

Float ShortEdgeRemoval::compute_edge_length(const Edge& e) const {
    size_t i1 = e.get_ori_data()[0];
    size_t i2 = e.get_ori_data()[1];
    VectorF v1 = get_vertex(i1);
    VectorF v2 = get_vertex(i2);
    return (v1 - v2).norm();
}

size_t ShortEdgeRemoval::get_num_vertices() const {
    const size_t num_ori_vertices = m_vertices.rows();
    const size_t num_new_vertices = m_new_vertices.size();
    const size_t num_vertices = num_ori_vertices + num_new_vertices;
    return num_vertices;
}

size_t ShortEdgeRemoval::get_num_faces() const {
    return m_faces.rows();
}

size_t ShortEdgeRemoval::get_dim() const {
    return m_vertices.cols();
}

