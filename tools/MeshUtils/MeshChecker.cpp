#include "MeshChecker.h"

#include <iostream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Math/MatrixUtils.h>

#include "Boundary.h"
#include "EdgeUtils.h"
#include "MeshSeparator.h"

MeshChecker::MeshChecker(const MatrixFr& vertices, const MatrixIr& faces)
    : m_vertices(vertices), m_faces(faces) {
        init_boundary();
        init_boundary_loops();
        init_edge_face_adjacency();
}

bool MeshChecker::is_manifold() const {
    for (auto adj : m_edge_face_adjacency) {
        if (adj.second.size() > 2) {
            return false;
        }
    }
    if ((2 - int(get_euler_characteristic())
                - int(get_num_boundary_loops())) % 2 != 0)
        return false;
    return true;
}

bool MeshChecker::is_closed() const {
    return m_boundary_edges.rows() == 0;
}

size_t MeshChecker::get_num_boundary_edges() const {
    return m_boundary_edges.rows();
}

size_t MeshChecker::get_num_boundary_loops() const {
    return m_num_bd_loops;
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
    int num_edges = m_edge_face_adjacency.size();
    int num_faces = m_faces.rows();
    return num_vertices - num_edges + num_faces;
}

size_t MeshChecker::get_num_connected_components() const {
    MeshSeparator separator(m_faces);
    return separator.separate();
}

size_t MeshChecker::get_num_isolated_vertices() const {
    const size_t num_vertices = m_vertices.rows();
    std::vector<bool> visited(num_vertices, false);
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_faces.row(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            visited[f[j]] = true;
        }
    }
    return std::count(visited.begin(), visited.end(), false);
}

size_t MeshChecker::get_num_duplicated_faces() const {
    const size_t num_faces = m_faces.rows();
    auto hash_fn = [](const VectorI& key) {
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
        auto itr = face_counts.find(f);
        if (itr == face_counts.end()) {
            face_counts.insert(itr, std::make_pair(f, 1));
        } else {
            itr->second ++;
        }
    }

    return std::count_if(face_counts.begin(), face_counts.end(),
            [](FaceMap::value_type& p) {
            return p.second != 1; });
}

void MeshChecker::init_boundary() {
    Boundary::Ptr bd = Boundary::extract_surface_boundary_raw(
            m_vertices, m_faces);
    m_boundary_edges = bd->get_boundaries();
}

void MeshChecker::init_boundary_loops() {
    m_num_bd_loops = std::numeric_limits<size_t>::max();
    try {
        auto chains = EdgeUtils::chain_edges(m_boundary_edges);
        m_num_bd_loops = chains.size();
    } catch (RuntimeError& e) {
        std::cerr << e.what() << std::endl;
    }
}

void MeshChecker::init_edge_face_adjacency() {
    m_edge_face_adjacency = EdgeUtils::compute_edge_face_adjacency(m_faces);
}

