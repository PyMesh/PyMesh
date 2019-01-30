/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeSquaredLengthAttribute.h"

#include <Mesh.h>
#include <tbb/tbb.h>

using namespace PyMesh;

void EdgeSquaredLengthAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& edge_sq_len = m_values;
    edge_sq_len = VectorF::Zero(num_faces * num_vertex_per_face);

    auto compute_kernel = [this, &mesh, &edge_sq_len, &num_vertex_per_face](
            const tbb::blocked_range<size_t>& r) {
        for (size_t i=r.begin(); i<r.end(); i++) {
            VectorF face_edge_sq_len =
                this->compute_edge_squared_length_on_face(mesh, i);
            edge_sq_len.segment(i*num_vertex_per_face,
                    num_vertex_per_face) = face_edge_sq_len;
        }
    };
    tbb::parallel_for(tbb::blocked_range<size_t>(0, num_faces), compute_kernel);
}

VectorF EdgeSquaredLengthAttribute::compute_edge_squared_length_on_face(
        Mesh& mesh, size_t face_idx) {
    const auto vertex_per_face = mesh.get_vertex_per_face();
    const auto& faces = mesh.get_faces();
    const auto& face = faces.segment(face_idx*vertex_per_face, vertex_per_face);
    VectorF len = VectorF::Zero(vertex_per_face);
    for (size_t i=0; i<vertex_per_face; i++) {
        size_t j = (i+1)%vertex_per_face;
        VectorF edge = mesh.get_vertex(face[i]) - mesh.get_vertex(face[j]);
        len[i] = edge.squaredNorm();
    }
    return len;
}
