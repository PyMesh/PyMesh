/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshRefiner.h"

#include <cassert>

using namespace PyMesh;

MeshRefiner::MeshRefiner(const std::string& method) {
    m_refiner = Subdivision::create(method);
}

VectorI MeshRefiner::update_face_sources(const VectorI& face_sources) const {
    VectorI face_indices = m_refiner->get_face_indices();
    const size_t num_faces = m_refiner->get_num_faces();
    assert(face_indices.size() == num_faces);

    VectorI result(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        assert(face_indices[i] < face_sources.size());
        result[i] = face_sources[face_indices[i]];
    }
    return result;
}

MatrixFr MeshRefiner::update_vertex_field(const MatrixFr& field) const {
    const auto& subdiv_matrices = m_refiner->get_subdivision_matrices();
    MatrixFr result = field;
    for (const auto& mat : subdiv_matrices) {
        result = mat * result;
    }
    return result;
}
