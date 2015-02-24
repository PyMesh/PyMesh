#include "MeshChecker.h"

#include <iostream>

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

bool MeshChecker::is_manifold() {
    for (auto adj : m_edge_face_adjacency) {
        if (adj.second.size() > 2) {
            return false;
        }
    }
    if ((2 - int(get_euler_characteristic())
                - int(get_num_boundary_loops())) % 2 == 1)
        return false;
    return true;
}

bool MeshChecker::is_closed() {
    return m_boundary_edges.rows() == 0;
}

size_t MeshChecker::get_num_boundary_edges() {
    return m_boundary_edges.rows();
}

size_t MeshChecker::get_num_boundary_loops() {
    return m_num_bd_loops;
}

int MeshChecker::get_genus() {
    // Need num boundary loops
    int double_genus = 2 - int(get_euler_characteristic()) -
        int(get_num_boundary_loops());
    if (double_genus % 2 == 1) {
        std::cerr << "Genus computation is incorrect due to nonmanifold surface"
            <<std::endl;
    }
    return double_genus / 2;
}

int MeshChecker::get_euler_characteristic() {
    int num_vertices = m_vertices.rows();
    int num_edges = m_edge_face_adjacency.size();
    int num_faces = m_faces.rows();
    return num_vertices - num_edges + num_faces;
}

size_t MeshChecker::get_num_connected_components() {
    MeshSeparator separator(m_faces);
    return separator.separate();
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

