/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshChecker.h"

#include <iostream>
#include <numeric>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Math/MatrixUtils.h>

#include "Boundary.h"
#include "EdgeUtils.h"
#include "MeshSeparator.h"

using namespace PyMesh;

MeshChecker::MeshChecker(const MatrixFr& vertices, const MatrixIr& faces,
        const MatrixIr& voxels)
    : m_vertices(vertices), m_faces(faces), m_voxels(voxels) {
        init_boundary();
        init_boundary_loops();
        init_edge_face_adjacency();
}

bool MeshChecker::is_vertex_manifold() const {
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();

    std::vector<std::vector<VectorI> > opposite_edges(num_vertices);
    if (vertex_per_face == 3) {
        for (size_t i=0; i<num_faces; i++) {
            const auto& f = m_faces.row(i);
            opposite_edges[f[0]].push_back(Vector2I(f[1], f[2]));
            opposite_edges[f[1]].push_back(Vector2I(f[2], f[0]));
            opposite_edges[f[2]].push_back(Vector2I(f[0], f[1]));
        }
    } else if (vertex_per_face == 4) {
        for (size_t i=0; i<num_faces; i++) {
            const auto& f = m_faces.row(i);
            opposite_edges[f[0]].push_back(Vector2I(f[1], f[2]));
            opposite_edges[f[0]].push_back(Vector2I(f[2], f[3]));
            opposite_edges[f[1]].push_back(Vector2I(f[2], f[3]));
            opposite_edges[f[1]].push_back(Vector2I(f[3], f[0]));
            opposite_edges[f[2]].push_back(Vector2I(f[3], f[0]));
            opposite_edges[f[2]].push_back(Vector2I(f[0], f[1]));
            opposite_edges[f[3]].push_back(Vector2I(f[0], f[1]));
            opposite_edges[f[3]].push_back(Vector2I(f[1], f[2]));
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Vertex manifold check does not support face with "
            << vertex_per_face << " vertices.";
        throw NotImplementedError(err_msg.str());
    }

    for (const auto& entries: opposite_edges) {
        if (entries.empty()) continue;
        try {
            auto edge_loops = EdgeUtils::chain_edges(
                    MatrixUtils::rowstack(entries));
            if (edge_loops.size() != 1) return false;
        } catch (...) {
            return false;
        }
    }
    return true;
}

bool MeshChecker::is_edge_manifold() const {
    for (auto adj : m_edge_face_adjacency) {
        if (adj.second.size() > 2) {
            return false;
        }
    }
    return true;
}

bool MeshChecker::is_oriented() const {
    // For each edge (s, d), check to see if the number of faces containing
    // (s, d) as an edge and the number of faces containing (d, s) as an edge
    // are equal.
    //
    // Boundary edges are skipped.
    const size_t num_vertex_per_face = m_faces.cols();
    for (auto adj : m_edge_face_adjacency) {
        if (adj.second.size() == 1) continue;

        const auto& e = adj.first.get_ori_data();
        if (e[0] == e[1]) {
            // It is impossible to determine the orientaiton of faces such as
            // [a, b, b] or [a, a, a].
            return false;
        } else {
            int consistent_count = 0;
            for (auto fid : adj.second) {
                VectorI f = m_faces.row(fid);
                for (size_t i=0; i<num_vertex_per_face; i++) {
                    if (f[i] == e[0] && f[(i+1)%num_vertex_per_face] == e[1]) {
                        consistent_count++;
                    } else if (f[i] == e[1] &&
                            f[(i+1)%num_vertex_per_face] == e[0]) {
                        consistent_count--;
                    }
                }
            }
            if (consistent_count != 0) return false;
        }
    }
    return true;
}

bool MeshChecker::is_closed() const {
    return m_boundary_edges.rows() == 0;
}

bool MeshChecker::has_edge_with_odd_adj_faces() const {
    for (auto adj : m_edge_face_adjacency) {
        if (adj.second.size() % 2 != 0) {
            return true;
        }
    }
    return false;
}

size_t MeshChecker::get_num_boundary_edges() const {
    return m_boundary_edges.rows();
}

size_t MeshChecker::get_num_boundary_loops() const {
    return m_boundary_loops.size();
}

int MeshChecker::get_genus() const {
    // Need num boundary loops
    int double_genus = 2 - int(get_euler_characteristic()) -
        int(get_num_boundary_loops());
    if (double_genus % 2 != 0) {
        std::cerr << "Genus computation is incorrect due to nonmanifold surface"
            <<std::endl;
    }
    return double_genus / 2;
}

int MeshChecker::get_euler_characteristic() const {
    int num_vertices = m_vertices.rows();
    if (m_voxels.rows() > 0) {
        // Only count surface vertices.
        std::vector<bool> on_surface(num_vertices, false);
        std::for_each(m_faces.data(), m_faces.data() + m_faces.size(),
                [&](int i) { on_surface[i] = true; } );
        num_vertices = std::accumulate(on_surface.begin(),
                on_surface.end(), 0);
    }
    const int num_edges = m_edge_face_adjacency.size();
    const int num_faces = m_faces.rows();
    return num_vertices - num_edges + num_faces;
}

size_t MeshChecker::get_num_connected_components() const {
    MeshSeparator separator(m_faces);
    separator.set_connectivity_type(MeshSeparator::VERTEX);
    return separator.separate();
}

size_t MeshChecker::get_num_connected_surface_components() const {
    MeshSeparator separator(m_faces);
    separator.set_connectivity_type(MeshSeparator::FACE);
    return separator.separate();
}

size_t MeshChecker::get_num_connected_volume_components() const {
    MeshSeparator separator(m_voxels);
    separator.set_connectivity_type(MeshSeparator::VOXEL);
    return separator.separate();
}

size_t MeshChecker::get_num_isolated_vertices() const {
    const size_t num_vertices = m_vertices.rows();
    std::vector<bool> visited(num_vertices, false);
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            visited[m_faces(i,j)] = true;
        }
    }

    const size_t num_voxels = m_voxels.rows();
    const size_t vertex_per_voxel = m_voxels.cols();
    for (size_t i=0; i<num_voxels; i++) {
        for (size_t j=0; j<vertex_per_voxel; j++) {
            visited[m_voxels(i,j)] = true;
        }
    }
    return std::count(visited.begin(), visited.end(), false);
}

size_t MeshChecker::get_num_duplicated_faces() const {
    const size_t num_faces = m_faces.rows();
    auto hash_fn = [](const VectorI& key) -> size_t {
        const size_t size = key.size();
        size_t hash = 0;
        for (size_t i=0; i<size; i++)
            hash += std::hash<int>()(key[i]);
        return hash;
    };
    typedef std::unordered_map<VectorI, size_t, decltype(hash_fn)> FaceMap;
    FaceMap face_counts( num_faces, hash_fn);

    const size_t vertex_per_face = m_faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_faces.row(i);
        Vector3I key(f.minCoeff(), f.maxCoeff(), f.sum());
        auto itr = face_counts.find(key);
        if (itr == face_counts.end()) {
            face_counts.insert(itr, std::make_pair(key, 1));
        } else {
            itr->second ++;
        }
    }

    return std::count_if(face_counts.begin(), face_counts.end(),
            [](FaceMap::value_type& p) {
            return p.second != 1; });
}

Float MeshChecker::compute_signed_volume_from_surface() const {
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    auto triangle_origin_volume = [](
            const Vector3F& v0, const Vector3F& v1, const Vector3F& v2) {
        return (v0.cross(v1)).dot(v2);
    };

    Float volume = 0.0;
    if (vertex_per_face == 3) {
        for (size_t i=0; i<num_faces; i++) {
            const auto& f = m_faces.row(i);
            Vector3F v0(
                    m_vertices.coeff(f.coeff(0), 0), 
                    m_vertices.coeff(f.coeff(0), 1), 
                    m_vertices.coeff(f.coeff(0), 2) );
            Vector3F v1(
                    m_vertices.coeff(f.coeff(1), 0), 
                    m_vertices.coeff(f.coeff(1), 1), 
                    m_vertices.coeff(f.coeff(1), 2) );
            Vector3F v2(
                    m_vertices.coeff(f.coeff(2), 0), 
                    m_vertices.coeff(f.coeff(2), 1), 
                    m_vertices.coeff(f.coeff(2), 2) );
            volume += triangle_origin_volume(v0, v1, v2);
        }
    } else if (vertex_per_face == 4) {
        for (size_t i=0; i<num_faces; i++) {
            const auto& f = m_faces.row(i);
            Vector3F v0(
                    m_vertices.coeff(f.coeff(0), 0), 
                    m_vertices.coeff(f.coeff(0), 1), 
                    m_vertices.coeff(f.coeff(0), 2) );
            Vector3F v1(
                    m_vertices.coeff(f.coeff(1), 0), 
                    m_vertices.coeff(f.coeff(1), 1), 
                    m_vertices.coeff(f.coeff(1), 2) );
            Vector3F v2(
                    m_vertices.coeff(f.coeff(2), 0), 
                    m_vertices.coeff(f.coeff(2), 1), 
                    m_vertices.coeff(f.coeff(2), 2) );
            Vector3F v3(
                    m_vertices.coeff(f.coeff(3), 0), 
                    m_vertices.coeff(f.coeff(3), 1), 
                    m_vertices.coeff(f.coeff(3), 2) );
            Vector3F center = (v0 + v1 + v2 + v3)/4.0;

            volume += triangle_origin_volume(v0, v1, center);
            volume += triangle_origin_volume(v1, v2, center);
            volume += triangle_origin_volume(v2, v3, center);
            volume += triangle_origin_volume(v3, v0, center);
        }
    }
    return volume / 6.0;
}

void MeshChecker::init_boundary() {
    Boundary::Ptr bd = Boundary::extract_surface_boundary_raw(
            m_vertices, m_faces);
    m_boundary_edges = bd->get_boundaries();
}

void MeshChecker::init_boundary_loops() {
    m_complex_bd = false;
    try {
        m_boundary_loops = EdgeUtils::chain_edges(m_boundary_edges);
    } catch (RuntimeError& e) {
        m_complex_bd = true;
        std::cerr << "Warning: " << e.what() << std::endl;
    }
}

void MeshChecker::init_edge_face_adjacency() {
    m_edge_face_adjacency = EdgeUtils::compute_edge_face_adjacency(m_faces);
}

