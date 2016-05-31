/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SimpleSubdivision.h"

#include <Core/Exception.h>

using namespace PyMesh;

void SimpleSubdivision::subdivide(
        MatrixFr vertices, MatrixIr faces, size_t num_iterations) {
    if (faces.cols() != 3) {
        throw NotImplementedError(
                "Only triangles mesh subdivision are supported!");
    }
    m_vertices = vertices;
    m_faces = faces;
    initialize_face_indices();

    for (size_t i=0; i<num_iterations; i++) {
        subdivide_once();
    }
}

void SimpleSubdivision::initialize_face_indices() {
    const size_t num_faces = m_faces.rows();
    m_face_indices.resize(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        m_face_indices[i] = i;
    }
}

void SimpleSubdivision::subdivide_once() {
    compute_subdivided_vertices();
    extract_sub_faces();
}

void SimpleSubdivision::compute_subdivided_vertices() {
    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    extract_edges();
    const size_t num_vertices = m_vertices.rows();
    for (size_t i=0; i<num_vertices; i++) {
        entries.push_back(T(i, i, 1.0));
    }

    for (auto itr : m_edge_index_map) {
        const VectorI& edge = itr.first.get_ori_data();
        const size_t edge_index = itr.second;
        entries.push_back(T(edge_index, edge[0], 0.5));
        entries.push_back(T(edge_index, edge[1], 0.5));
    }

    const size_t num_edges = m_edge_index_map.size();
    ZSparseMatrix subdiv_mat(num_vertices + num_edges, num_vertices);
    subdiv_mat.setFromTriplets(entries.begin(), entries.end());

    m_vertices = subdiv_mat * m_vertices;
    m_subdivision_matrices.push_back(subdiv_mat);
}

void SimpleSubdivision::extract_sub_faces() {
    std::vector<VectorI> sub_faces;
    std::vector<size_t> sub_face_indices;

    const size_t num_faces = m_faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = m_faces.row(i);
        Vector3I mid_edge_idx = get_edge_indices(face);

        sub_faces.push_back(Vector3I(face[0], mid_edge_idx[0], mid_edge_idx[2]));
        sub_faces.push_back(Vector3I(face[1], mid_edge_idx[1], mid_edge_idx[0]));
        sub_faces.push_back(Vector3I(face[2], mid_edge_idx[2], mid_edge_idx[1]));
        sub_faces.push_back(Vector3I(mid_edge_idx[0], mid_edge_idx[1], mid_edge_idx[2]));

        sub_face_indices.push_back(m_face_indices[i]);
        sub_face_indices.push_back(m_face_indices[i]);
        sub_face_indices.push_back(m_face_indices[i]);
        sub_face_indices.push_back(m_face_indices[i]);
    }

    assert(sub_faces.size() == 4 * num_faces);
    const size_t num_sub_faces = 4 * num_faces;

    m_faces.resize(num_sub_faces, 3);
    for (size_t i=0; i<num_sub_faces; i++) {
        m_faces.row(i) = sub_faces[i];
    }

    m_face_indices.resize(num_sub_faces);
    std::copy(sub_face_indices.begin(), sub_face_indices.end(),
            m_face_indices.data());
}

void SimpleSubdivision::extract_edges() {
    const size_t num_faces = m_faces.rows();
    const size_t base_index = m_vertices.rows();

    m_edge_index_map.clear();
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = m_faces.row(i);
        register_edges(face, base_index);
    }
}

void SimpleSubdivision::register_edges(const VectorI& face, size_t base_index) {
    const size_t vertex_per_face = face.size();
    for (size_t i=0; i<vertex_per_face; i++) {
        Triplet edge(face[i], face[(i+1)%vertex_per_face]);
        auto itr = m_edge_index_map.find(edge);
        if (itr == m_edge_index_map.end()) {
            size_t mid_edge_index = base_index + m_edge_index_map.size();
            m_edge_index_map[edge] = mid_edge_index;
        }
    }
}

Vector3I SimpleSubdivision::get_edge_indices(const VectorI& face) {
    typedef std::map<Triplet, size_t>::const_iterator EdgeMapIterator;
    EdgeMapIterator edges[3] = {
        m_edge_index_map.find(Triplet(face[0], face[1])),
        m_edge_index_map.find(Triplet(face[1], face[2])),
        m_edge_index_map.find(Triplet(face[2], face[0]))
    };

    assert(edges[0] != m_edge_index_map.end());
    assert(edges[1] != m_edge_index_map.end());
    assert(edges[2] != m_edge_index_map.end());

    Vector3I mid_edge_idx(
            edges[0]->second, edges[1]->second, edges[2]->second);
    return mid_edge_idx;
}


