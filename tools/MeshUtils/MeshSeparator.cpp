#include "MeshSeparator.h"

#include <cassert>
#include <list>
#include <queue>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include "Edge.h"
#include "EdgeMap.h"

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
    for (size_t i=0; i<num_faces; i++) {
        Vector3I face = m_faces.row(i);
        Edge e1(face[0], face[1]);
        Edge e2(face[1], face[2]);
        Edge e3(face[2], face[0]);

        m_edge_map.insert(e1, i);
        m_edge_map.insert(e2, i);
        m_edge_map.insert(e3, i);
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

    MatrixI comp_faces(face_list.size(), 3);
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
    const Vector3I& face = m_faces.row(fi);
    Edge e1(face[0], face[1]);
    Edge e2(face[1], face[2]);
    Edge e3(face[2], face[0]);

    AdjFaces& neighbor1 = m_edge_map[e1];
    AdjFaces& neighbor2 = m_edge_map[e2];
    AdjFaces& neighbor3 = m_edge_map[e3];

    const size_t num_neighbors =
        neighbor1.size() + neighbor2.size() + neighbor3.size();
    std::vector<size_t> result;
    result.reserve(num_neighbors);

    result.insert(result.end(), neighbor1.begin(), neighbor1.end());
    result.insert(result.end(), neighbor2.begin(), neighbor2.end());
    result.insert(result.end(), neighbor3.begin(), neighbor3.end());

    return result;
}

void MeshSeparator::clear() {
    m_components.clear();
    m_visited.clear();
    m_edge_map.clear();
}
