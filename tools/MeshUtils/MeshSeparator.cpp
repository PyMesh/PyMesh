#include "MeshSeparator.h"

#include <cassert>
#include <list>
#include <queue>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>

#include "TripletMap.h"

MeshSeparator::MeshSeparator(const MatrixI& faces) : m_faces(faces) {
    compute_face_connectivity();
}

size_t MeshSeparator::separate() {
    const size_t num_faces = m_faces.rows();

    m_visited = std::vector<bool>(num_faces, false);

    for (size_t i=0; i<num_faces; i++) {
        if (m_visited[i]) continue;
        MatrixI comp_f = flood(i);
        m_components.push_back(comp_f);
    }

    return m_components.size();
}

void MeshSeparator::compute_face_connectivity() {
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_faces.row(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            m_edge_map.insert(Edge(f[j], f[(j+1)%vertex_per_face]), i);
        }
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

    const size_t vertex_per_face = m_faces.cols();
    MatrixI comp_faces(face_list.size(), vertex_per_face);
    size_t count = 0;
    for (std::list<size_t>::const_iterator itr = face_list.begin();
            itr != face_list.end(); itr++) {
        assert(count < m_faces.rows());
        assert(*itr < m_faces.rows());
        comp_faces.row(count) = m_faces.row(*itr);
        count++;
    }
    return comp_faces;
}

std::vector<size_t> MeshSeparator::get_adjacent_faces(size_t fi) {
    std::vector<size_t> neighbors;
    const auto& face = m_faces.row(fi);
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
