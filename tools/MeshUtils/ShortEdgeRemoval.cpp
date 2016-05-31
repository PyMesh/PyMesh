/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ShortEdgeRemoval.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <set>

#include <Core/Exception.h>

#include "IndexHeap.h"

using namespace PyMesh;

const size_t ShortEdgeRemoval::UNMAPPED = std::numeric_limits<size_t>::max();
const Float  ShortEdgeRemoval::INFINITE = std::numeric_limits<Float>::max();

ShortEdgeRemoval::ShortEdgeRemoval(const MatrixFr& vertices, const MatrixIr& faces) :
    m_vertices(vertices),
    m_faces(faces),
    m_num_collapsed(0),
    m_heap(false) {
        const size_t vertex_per_face = m_faces.cols();
        if (vertex_per_face != 3) {
            throw NotImplementedError("Only triangle faces are supported!");
        }
        m_importance = VectorI::Zero(m_vertices.rows());
}

size_t ShortEdgeRemoval::run(Float threshold) {
    size_t num_collapsed = m_num_collapsed;
    init();
    do {
        num_collapsed = m_num_collapsed;
        collapse(threshold);
        update();
        if (num_collapsed == m_num_collapsed) break;
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

void ShortEdgeRemoval::init() {
    init_vertex_map();
    init_edges();
    init_edge_length_heap();
    init_face_indices();
    init_vertex_face_neighbors();
}

void ShortEdgeRemoval::update() {
    update_faces();
    update_importance();
    update_vertices();
    init_vertex_map();
    init_edges();
    init_edge_length_heap();
    init_vertex_face_neighbors();
}

void ShortEdgeRemoval::init_vertex_map() {
    const size_t num_vertices = get_num_vertices();
    m_vertex_map.resize(num_vertices);
    std::fill(m_vertex_map.begin(), m_vertex_map.end(), UNMAPPED);
}

void ShortEdgeRemoval::init_face_indices() {
    const size_t num_faces = get_num_faces();
    m_face_indices.resize(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        m_face_indices[i] = i;
    }
}

void ShortEdgeRemoval::init_vertex_face_neighbors() {
    m_vertex_face_neighbors.clear();
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    assert(num_vertices == get_num_vertices());
    assert(num_faces == get_num_faces());

    m_vertex_face_neighbors.resize(num_vertices);
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            m_vertex_face_neighbors[m_faces(i,j)].push_back(i);
        }
    }
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
        if (edge_can_be_collapsed(i)) {
            edge_lengths[i] = compute_edge_length(m_edges[i]);
        } else {
            edge_lengths[i] = INFINITE;
        }
    }
    m_heap.init(edge_lengths);
}

void ShortEdgeRemoval::update_vertices() {
    m_vertices = get_vertices();
    m_new_vertices.clear();
}

void ShortEdgeRemoval::update_faces() {
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    const size_t num_entries = num_faces * vertex_per_face;

    std::vector<size_t> faces;
    std::vector<size_t> face_indices;
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = m_faces.row(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t mapped_idx = m_vertex_map[face[j]];
            if (mapped_idx != UNMAPPED)
                face[j] = mapped_idx;
        }
        if (face[0] == face[1] ||
            face[1] == face[2] ||
            face[2] == face[0])
            continue;
        faces.insert(faces.end(), face.data(), face.data() + vertex_per_face);
        face_indices.push_back(m_face_indices[i]);
    }

    assert(faces.size() % vertex_per_face == 0);
    m_faces.resize(faces.size() / vertex_per_face, vertex_per_face);
    std::copy(faces.begin(), faces.end(), m_faces.data());

    m_face_indices.resize(face_indices.size());
    std::copy(face_indices.begin(), face_indices.end(), m_face_indices.data());
    assert(m_faces.rows() == m_face_indices.size());
}

void ShortEdgeRemoval::update_importance() {
    const size_t num_ori_vertices = m_vertex_map.size();
    const size_t num_vertices = get_num_vertices();
    m_importance.conservativeResize(num_vertices);
    m_importance.segment(num_ori_vertices,
            num_vertices-num_ori_vertices).setZero();
    for (size_t i=0; i<num_ori_vertices; i++) {
        size_t mapped_idx = m_vertex_map[i];
        if (mapped_idx != UNMAPPED) {
            int cur_val = m_importance[mapped_idx];
            int old_val = m_importance[i];
            assert(cur_val >= -1);
            assert(old_val >= -1);
            if (cur_val < 0 || old_val < 0) {
                m_importance[mapped_idx] = -1;
            } else {
                m_importance[mapped_idx] = std::max(cur_val, old_val);
            }
        }
    }
}

void ShortEdgeRemoval::collapse(Float threshold) {
    while (!m_heap.empty()) {
        size_t edge_idx = m_heap.top();
        Float edge_len = m_heap.top_value();
        m_heap.pop();

        if (edge_len > threshold) break;
        if (!edge_is_valid(edge_idx)) continue;
        if (!edge_can_be_collapsed(edge_idx)) continue;

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

bool ShortEdgeRemoval::edge_can_be_collapsed(size_t edge_idx) const {
    const Edge& edge = m_edges[edge_idx];
    size_t v1_idx = edge.get_ori_data()[0];
    size_t v2_idx = edge.get_ori_data()[1];
    return (m_importance[v1_idx] >= 0) || (m_importance[v2_idx] >= 0);
}

bool ShortEdgeRemoval::collapse_would_cause_fold_over(
        size_t edge_idx, const VectorF& v) const {
    const Edge& e = m_edges[edge_idx];
    const size_t i1 = e.get_ori_data()[0];
    const size_t i2 = e.get_ori_data()[1];
    const auto& v1_face_neighbors = m_vertex_face_neighbors[i1];
    const auto& v2_face_neighbors = m_vertex_face_neighbors[i2];
    if (faces_would_flip(i1, i2, v, v1_face_neighbors)) return true;
    if (faces_would_flip(i1, i2, v, v2_face_neighbors)) return true;
    return false;
}

bool ShortEdgeRemoval::faces_would_flip(size_t i1, size_t i2,
        const VectorF& v,
        const std::vector<size_t>& faces) const {
    auto index_of = [=](const Vector3I& array, size_t val) -> size_t {
        if (array[0] == val) return 0;
        if (array[1] == val) return 1;
        if (array[2] == val) return 2;
        return std::numeric_limits<size_t>::max();
    };
    for (const auto fi : faces) {
        const Vector3I& f = m_faces.row(fi);
        size_t local_i1 = index_of(f, i1);
        size_t local_i2 = index_of(f, i2);
        if (local_i1 < 3 && local_i2 < 3) {
            // This face contains the edge and would be eliminated after
            // collapse.
            continue;
        } else if (local_i1 < 3) {
            VectorF v_old = get_vertex(i1);
            const VectorF vo1 = get_vertex(f[(local_i1+1)%3]);
            const VectorF vo2 = get_vertex(f[(local_i1+2)%3]);
            if (face_would_flip(v_old, v, vo1, vo2)) { return true; }
        } else if (local_i2 < 3) {
            VectorF v_old = get_vertex(i2);
            const VectorF vo1 = get_vertex(f[(local_i2+1)%3]);
            const VectorF vo2 = get_vertex(f[(local_i2+2)%3]);
            if (face_would_flip(v_old, v, vo1, vo2)) { return true; }
        }
    }
    return false;
}

bool ShortEdgeRemoval::face_would_flip(
        const VectorF& v_old, const VectorF& v_new,
        const VectorF& v_o1,  const VectorF& v_o2) const {
    const size_t dim = get_dim();
    Vector3F en1, en2, eo1, eo2;
    en1 = en2 = eo1 = eo2 = Vector3F::Zero();
    en1.segment(0, dim) = v_o1 - v_new;
    en2.segment(0, dim) = v_o2 - v_new;
    eo1.segment(0, dim) = v_o1 - v_old;
    eo2.segment(0, dim) = v_o2 - v_old;
    Vector3F normal_new = en1.cross(en2).normalized();
    Vector3F normal_old = eo1.cross(eo2).normalized();
    if (!normal_old.allFinite()) {
        // If old triangle is degenerated, things won't get worse if we
        // collapse.
        return false;
    }
    if (!normal_new.allFinite()) {
        // To not collapse if new Triangle is degenerated while old one is not.
        return true;
    }
    return normal_new.dot(normal_old) < 0.5;
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
    const int v1_importance = m_importance[i1];
    const int v2_importance = m_importance[i2];

    VectorF new_v;
    if (v1_importance < 0) {
        assert(v2_importance >= 0);
        new_v = v1;
    } else if (v2_importance < 0) {
        assert(v1_importance >= 0);
        new_v = v2;
    } else {
        if (v1_importance == v2_importance) {
            new_v = 0.5 * (v1 + v2);
        } else if (v1_importance > v2_importance) {
            new_v = v1;
        } else {
            new_v = v2;
        }
    }

    if (collapse_would_cause_fold_over(edge_idx, new_v)) { return; }

    m_new_vertices.push_back(new_v);
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

