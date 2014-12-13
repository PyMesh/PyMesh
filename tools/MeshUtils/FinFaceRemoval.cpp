#include "FinFaceRemoval.h"

#include <list>

#include <Core/Exception.h>
#include <Misc/Triplet.h>

#include "TripletMap.h"

FinFaceRemoval::FinFaceRemoval(const MatrixFr& vertices, const MatrixIr& faces)
    : m_vertices(vertices), m_faces(faces) {}

size_t FinFaceRemoval::run() {
    const size_t num_vertex_per_face = m_faces.cols();
    if (num_vertex_per_face != 3) {
        throw NotImplementedError("Fin face removal only support triangle face.");
    }

    const size_t num_faces = m_faces.rows();
    TripletMap<size_t> face_index_map;
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& f = m_faces.row(i);
        Triplet key(f[0], f[1], f[2]);
        face_index_map.insert(key, i);
    }

    std::list<int> face_list;
    std::list<size_t> face_indices;
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& f = m_faces.row(i);
        Triplet key(f[0], f[1], f[2]);

        const auto& indices = face_index_map[key];
        if (indices.size() == 1) {
            face_list.push_back(f[0]);
            face_list.push_back(f[1]);
            face_list.push_back(f[2]);
            face_indices.push_back(i);
        }
    }

    const size_t num_faces_left = face_indices.size();
    m_faces.resize(num_faces_left, 3);
    m_face_indices.resize(num_faces_left);
    std::copy(face_list.begin(), face_list.end(), m_faces.data());
    std::copy(face_indices.begin(), face_indices.end(), m_face_indices.data());

    return num_faces - num_faces_left;
}
