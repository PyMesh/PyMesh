/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeSquaredLengthAttribute.h"

#include <Mesh.h>

using namespace PyMesh;

void EdgeSquaredLengthAttribute::compute_from_mesh(Mesh& mesh) {
    size_t num_faces = mesh.get_num_faces();
    size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& edge_sq_len = m_values;
    edge_sq_len = VectorF::Zero(num_faces * num_vertex_per_face);

    for (size_t i=0; i<num_faces; i++) {
        VectorF face_edge_sq_len = compute_edge_squared_length_on_face(mesh, i);
        edge_sq_len.segment(i*num_vertex_per_face, num_vertex_per_face) =
            face_edge_sq_len;
    }
}

VectorF EdgeSquaredLengthAttribute::compute_edge_squared_length_on_face(
        Mesh& mesh, size_t face_idx) {
    VectorI face = mesh.get_face(face_idx);
    VectorF len = VectorF::Zero(face.size());
    for (size_t i=0; i<face.size(); i++) {
        size_t j = (i+1)%face.size();
        VectorF edge = mesh.get_vertex(face[i]) - mesh.get_vertex(face[j]);
        len[i] = edge.squaredNorm();
    }
    return len;
}
