#include "ShortEdgeRemoval.h"
#include "IndexHeap.h"

ShortEdgeRemoval::ShortEdgeRemoval(MatrixF& vertices, MatrixI& faces)
        : m_vertices(vertices), m_faces(faces) {}

size_t ShortEdgeRemoval::run(Float threshold) {
    init_vertex_map();
    compute_edge_lengths();

    size_t num_collapsed = collapse(threshold);
    return num_collapsed;
}

MatrixF ShortEdgeRemoval::get_vertices() {
    const size_t num_old_v = m_vertices.rows();
    const size_t num_new_v = m_new_vertices.size();
    const size_t num_vertices = num_old_v + num_new_v;
    MatrixF vertices(num_vertices, 3);
    vertices.topRows(num_old_v) = m_vertices;
    for (size_t i=0; i<num_new_v; i++) {
        vertices.row(i+num_old_v) = m_new_vertices[i];
    }
    return vertices;
}

MatrixI ShortEdgeRemoval::get_faces() {
    const size_t num_ori_faces = m_faces.rows();
    std::vector<Vector3I> mod_faces;
    mod_faces.reserve(num_ori_faces);
    for (size_t i=0; i<num_ori_faces; i++) {
        Vector3I f(
                m_vertex_map[m_faces(i, 0)],
                m_vertex_map[m_faces(i, 1)],
                m_vertex_map[m_faces(i, 2)]);
        if (f[0] != f[1] && f[0] != f[2] && f[1] != f[2]) {
            mod_faces.push_back(f);
        }
    }

    const size_t num_faces = mod_faces.size();
    MatrixI faces(num_faces, 3);
    for (size_t i=0; i<num_faces; i++) {
        faces.row(i) = mod_faces[i];
    }
    return faces;
}

void ShortEdgeRemoval::init_vertex_map() {
    const size_t num_vertices = m_vertices.rows();
    m_vertex_map.reserve(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        m_vertex_map.push_back(i);
    }
}

void ShortEdgeRemoval::compute_edge_lengths() {
    const size_t num_faces = m_faces.rows();
    m_edges.clear();
    m_edges.reserve(num_faces * 3);
    m_edge_lengths.clear();
    m_edge_lengths.reserve(num_faces * 3);
    for (size_t i=0; i<num_faces; i++) {
        Edge e1(m_faces(i, 0), m_faces(i, 1));
        Edge e2(m_faces(i, 1), m_faces(i, 2));
        Edge e3(m_faces(i, 2), m_faces(i, 0));

        m_edges.push_back(e1);
        m_edges.push_back(e2);
        m_edges.push_back(e3);

        Float l1 = compute_edge_length(e1);
        Float l2 = compute_edge_length(e2);
        Float l3 = compute_edge_length(e3);

        m_edge_lengths.push_back(l1);
        m_edge_lengths.push_back(l2);
        m_edge_lengths.push_back(l3);
    }
}

size_t ShortEdgeRemoval::collapse(Float threshold) {
    size_t num_collapsed = 0;
    IndexHeap<Float> candidates(m_edge_lengths, false);
    while (!candidates.empty()) {
        size_t ext_idx = candidates.top();
        candidates.pop();

        if (m_edge_lengths[ext_idx] > threshold) break;
        if (!can_be_collapsed(ext_idx)) continue;

        collapse_edge(ext_idx);
        num_collapsed ++;
    }
    return num_collapsed;
}

bool ShortEdgeRemoval::can_be_collapsed(size_t ext_idx) const {
    const Edge& e = m_edges[ext_idx];
    return  (m_vertex_map.at(e.m_v1) == e.m_v1)
        and (m_vertex_map.at(e.m_v2) == e.m_v2);
}

void ShortEdgeRemoval::collapse_edge(size_t ext_idx) {
    const Edge& e = m_edges[ext_idx];
    const size_t i1 = e.m_v1;
    const size_t i2 = e.m_v2;
    Vector3F v1 = m_vertices.row(i1);
    Vector3F v2 = m_vertices.row(i2);
    Vector3F v_mid = 0.5 * (v1 + v2);
    m_new_vertices.push_back(v_mid);

    size_t idx_mid = m_vertices.rows() + m_new_vertices.size() - 1;
    m_vertex_map[i1] = idx_mid;
    m_vertex_map[i2] = idx_mid;
}

Float ShortEdgeRemoval::compute_edge_length(const Edge& e) const {
    const Vector3F& v1 = m_vertices.row(e.m_v1);
    const Vector3F& v2 = m_vertices.row(e.m_v2);
    return (v1 - v2).norm();
}

