/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshSeparator.h"

#include <cassert>
#include <list>
#include <queue>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Misc/TripletMap.h>

MeshSeparator::MeshSeparator(const MatrixI& elements) : m_elements(elements) {
    compute_face_connectivity();
}

size_t MeshSeparator::separate() {
    const size_t num_faces = m_elements.rows();

    m_visited = std::vector<bool>(num_faces, false);

    for (size_t i=0; i<num_faces; i++) {
        if (m_visited[i]) continue;
        MatrixI comp_f = flood(i);
        m_components.push_back(comp_f);
    }

    return m_components.size();
}

void MeshSeparator::compute_face_connectivity() {
    const size_t num_faces = m_elements.rows();
    const size_t vertex_per_face = m_elements.cols();
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_elements.row(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            m_edge_map.insert(Edge(f[j], f[(j+1)%vertex_per_face]), i);
        }
    }
}

void MeshSeparator::compute_voxel_connectivity() {
    const size_t num_voxels = m_elements.rows();
    if (m_elements.cols() != 4) {
        throw RuntimeError("Only tetrahedron elements are supported");
    }
}

MatrixI MeshSeparator::flood(size_t seed) {
    std::queue<size_t> Q;
    Q.push(seed);
    m_visited[seed] = true;

    std::list<size_t> face_list;
    while (!Q.empty()) {
        size_t f = Q.front();
        Q.pop();
        face_list.push_back(f);

        std::vector<size_t> neighbors = get_adjacent_faces(f);
        for (size_t i=0; i<neighbors.size(); i++) {
            size_t f_next = neighbors[i];
            if (m_visited[f_next]) continue;
            Q.push(f_next);
            m_visited[f_next] = true;
        }
    }

    const size_t vertex_per_face = m_elements.cols();
    MatrixI comp_faces(face_list.size(), vertex_per_face);
    size_t count = 0;
    for (std::list<size_t>::const_iterator itr = face_list.begin();
            itr != face_list.end(); itr++) {
        assert(count < m_elements.rows());
        assert(*itr < m_elements.rows());
        comp_faces.row(count) = m_elements.row(*itr);
        count++;
    }
    return comp_faces;
}

std::vector<size_t> MeshSeparator::get_adjacent_faces(size_t fi) {
    std::vector<size_t> neighbors;
    const auto& face = m_elements.row(fi);
    const size_t vertex_per_face = face.size();
    for (size_t i=0; i<vertex_per_face; i++) {
        Edge e(face[i], face[(i+1)%vertex_per_face]);
        AdjFaces& neighbor = m_edge_map[e];
        neighbors.insert(neighbors.end(), neighbor.begin(), neighbor.end());
    }
    return neighbors;
}

void MeshSeparator::clear() {
    m_components.clear();
    m_visited.clear();
    m_edge_map.clear();
}
