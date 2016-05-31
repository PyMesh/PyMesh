/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FinFaceRemoval.h"

#include <cassert>
#include <list>

#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Misc/TripletMap.h>

using namespace PyMesh;

namespace FinFaceRemovalHelper {
    /**
     * Compute face orientation based on its vertex indices:
     * Return  1 if the face orientated the same way as (1, 2, 3).
     * Return -1 if the face orientated the same way as (1, 3, 2).
     * Return  0 if the face contains duplicated vertices as (1, 1, 2).
     */
    template<typename Derived>
    int orientation(const Eigen::DenseBase<Derived>& f) {
        assert(f.size() == 3);
        int result = 0;

        if (f[0] < f[1]) result++;
        else if (f[0] > f[1]) result--;

        if (f[1] < f[2]) result++;
        else if (f[1] > f[2]) result--;

        if (f[2] < f[0]) result++;
        else if (f[2] > f[0]) result--;

        return result;
    }

    /**
     * Return a face that has the correct orientation.
     */
    int compute_majority_orientation(
            const MatrixIr& faces, const std::vector<size_t>& face_indices,
            size_t& correctly_orientated_face_idx) {
        size_t positive_fid = std::numeric_limits<size_t>::max();
        size_t negative_fid = std::numeric_limits<size_t>::max();

        int majority_orientation = 0;
        for (size_t fid : face_indices) {
            int ori = orientation(faces.row(fid));
            majority_orientation += ori;
            if (ori > 0) positive_fid = fid;
            else if (ori < 0) negative_fid = fid;
        }

        if (majority_orientation > 0) {
            correctly_orientated_face_idx = positive_fid;
            assert(correctly_orientated_face_idx < faces.rows());
        } else if (majority_orientation < 0) {
            correctly_orientated_face_idx = negative_fid;
            assert(correctly_orientated_face_idx < faces.rows());
        }

        return majority_orientation;
    }
}
using namespace FinFaceRemovalHelper;

FinFaceRemoval::FinFaceRemoval(const MatrixFr& vertices, const MatrixIr& faces)
    : m_vertices(vertices), m_faces(faces), m_fins_only(false) {}

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
    std::vector<bool> visited(num_faces, false);
    for (size_t i=0; i<num_faces; i++) {
        if (visited[i]) continue;
        const VectorI& f = m_faces.row(i);
        if (f[0] == f[1] || f[1] == f[2] || f[2] == f[0]) {
            // Ignore combinatorially degenerated faces.
            continue;
        }
        Triplet key(f[0], f[1], f[2]);

        const auto& indices = face_index_map[key];
        size_t correctly_orientated_fid = std::numeric_limits<size_t>::max();
        const int majority_orientation = compute_majority_orientation(
                m_faces, indices, correctly_orientated_fid);

        if (majority_orientation != 0) {
            size_t duplicity = m_fins_only ? abs(majority_orientation) : 1;
            for (size_t j=0; j<duplicity; j++) {
                assert(correctly_orientated_fid < num_faces);
                const auto& cf = m_faces.row(correctly_orientated_fid).eval();
                face_list.push_back(cf[0]);
                face_list.push_back(cf[1]);
                face_list.push_back(cf[2]);
                face_indices.push_back(correctly_orientated_fid);
            }
        }

        std::for_each(indices.begin(), indices.end(), [&](size_t index)
                { visited[index] = true; });
    }

    const size_t num_faces_left = face_indices.size();
    m_faces.resize(num_faces_left, 3);
    m_face_indices.resize(num_faces_left);
    if (num_faces_left > 0) {
        std::copy(face_list.begin(), face_list.end(), m_faces.data());
        std::copy(face_indices.begin(), face_indices.end(), m_face_indices.data());
    }

    return num_faces - num_faces_left;
}
