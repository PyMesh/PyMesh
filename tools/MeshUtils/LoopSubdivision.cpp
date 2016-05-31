/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include <cmath>
#include <Core/Exception.h>
#include "LoopSubdivision.h"

using namespace PyMesh;

void LoopSubdivision::subdivide(
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

void LoopSubdivision::subdivide_once() {
    extract_edges();
    compute_vertex_valance();
    compute_subdivided_vertices();
    extract_sub_faces();
}

void LoopSubdivision::initialize_face_indices() {
    const size_t num_faces = m_faces.rows();
    m_face_indices.resize(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        m_face_indices[i] = i;
    }
}

void LoopSubdivision::extract_edges() {
    const size_t num_faces = m_faces.rows();
    const size_t base_index = m_vertices.rows();

    m_edge_index_map.clear();
    m_boundary_edge.clear();
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = m_faces.row(i);
        register_edges(face, base_index);
    }
}

void LoopSubdivision::compute_vertex_valance() {
    const size_t num_vertices = m_vertices.rows();
    m_vertex_valance = std::vector<size_t>(num_vertices, 0);
    for (auto itr : m_edge_index_map) {
        const VectorI& edge = itr.first.get_ori_data();
        m_vertex_valance[edge[0]]++;
        m_vertex_valance[edge[1]]++;
    }
}

void LoopSubdivision::extract_sub_faces() {
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

void LoopSubdivision::compute_subdivided_vertices() {
    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    std::vector<bool> on_boundary = compute_boundary_vertices();
    for (auto itr : m_edge_index_map) {
        const VectorI& edge = itr.first.get_ori_data();
        if (m_boundary_edge[itr.first] > 1) {
            size_t k_0 = m_vertex_valance[edge[0]];
            size_t k_1 = m_vertex_valance[edge[1]];
            Float beta_0 = compute_beta(k_0);
            Float beta_1 = compute_beta(k_1);

            if (!on_boundary[edge[0]]) {
                entries.push_back(T(edge[0], edge[0], 1.0/k_0 - beta_0));
                entries.push_back(T(edge[0], edge[1], beta_0));
            }
            if (!on_boundary[edge[1]]) {
                entries.push_back(T(edge[1], edge[1], 1.0/k_1 - beta_1));
                entries.push_back(T(edge[1], edge[0], beta_1));
            }
        } else {
            assert(m_boundary_edge[itr.first] == 1);
            entries.push_back(T(edge[0], edge[0], 3.0/8.0));
            entries.push_back(T(edge[0], edge[1], 1.0/8.0));
            entries.push_back(T(edge[1], edge[1], 3.0/8.0));
            entries.push_back(T(edge[1], edge[0], 1.0/8.0));
        }
    }

    const size_t num_faces = m_faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& face = m_faces.row(i);
        for (size_t j=0; j<3; j++) {
            Triplet edge(face[j], face[(j+1)%3]);
            size_t mid_edge_idx = m_edge_index_map[edge];
            if (m_boundary_edge[edge] > 1) {
                entries.push_back(T(mid_edge_idx, face[(j+2)%3], 1.0/8.0));
                entries.push_back(T(mid_edge_idx, face[(j+0)%3], 3.0/16.0));
                entries.push_back(T(mid_edge_idx, face[(j+1)%3], 3.0/16.0));
            } else {
                assert(m_boundary_edge[edge] == 1);
                entries.push_back(T(mid_edge_idx, face[j], 0.5));
                entries.push_back(T(mid_edge_idx, face[(j+1)%3], 0.5));
            }
        }
    }

    const size_t num_edges = m_edge_index_map.size();
    const size_t num_vertices_before = m_vertices.rows();
    const size_t num_vertices_after  = num_vertices_before + num_edges;
    ZSparseMatrix subdiv_mat(num_vertices_after, num_vertices_before);
    subdiv_mat.setFromTriplets(entries.begin(), entries.end());

    m_vertices = subdiv_mat * m_vertices;
    m_subdivision_matrices.push_back(subdiv_mat);
}

void LoopSubdivision::register_edges(const VectorI& face, size_t base_index) {
    const size_t vertex_per_face = face.size();
    for (size_t i=0; i<vertex_per_face; i++) {
        Triplet edge(face[i], face[(i+1)%vertex_per_face]);
        auto itr = m_edge_index_map.find(edge);
        if (itr == m_edge_index_map.end()) {
            size_t mid_edge_index = base_index + m_edge_index_map.size();
            m_edge_index_map[edge] = mid_edge_index;
            m_boundary_edge[edge] = 1;
        } else {
            assert(m_boundary_edge.find(edge) != m_boundary_edge.end());
            assert(m_boundary_edge[edge] == 1);
            m_boundary_edge[edge]++;
        }
    }
}

Float LoopSubdivision::compute_beta(size_t valance) {
    return (5.0 / 8.0 - pow(3 + 2.0 * cos(2 * M_PI / valance), 2) / 64.0) / Float(valance);
}

Vector3I LoopSubdivision::get_edge_indices(const VectorI& face) {
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

std::vector<bool> LoopSubdivision::compute_boundary_vertices() const {
    const size_t num_vertices = m_vertices.rows();
    std::vector<bool> on_boundary(num_vertices, false);
    for (auto itr : m_boundary_edge) {
        if (itr.second <= 1) {
            const VectorI& edge = itr.first.get_ori_data();
            on_boundary[edge[0]] = true;
            on_boundary[edge[1]] = true;
        }
    }
    return on_boundary;
}
