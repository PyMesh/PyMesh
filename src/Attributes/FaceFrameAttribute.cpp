/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "FaceFrameAttribute.h"
#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void FaceFrameAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    switch (dim) {
        case 2:
            compute_2D_frame_field(mesh);
            break;
        case 3:
            compute_3D_frame_field(mesh);
            break;
        default:
            throw NotImplementedError("Unsupported dimension "
                    + std::to_string(dim));
    }
}

void FaceFrameAttribute::compute_2D_frame_field(Mesh& mesh) {
    assert(mesh.get_dim() == 2);
    const size_t num_faces = mesh.get_num_faces();
    m_values.resize(num_faces * 4); // 2x2 matrix per face.
    for (size_t i=0; i<num_faces; i++) {
        m_values.segment<4>(i*4) << 1, 0, 0, 1;
    }
    return;
}

void FaceFrameAttribute::compute_3D_frame_field(Mesh& mesh) {
    assert(mesh.get_dim() == 3);
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();
    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    if (!mesh.has_attribute("face_normal")) {
        mesh.add_attribute("face_normal");
    }

    const auto& faces = mesh.get_faces();
    const auto& edge_lengths = mesh.get_attribute("edge_length");
    const auto& normals = mesh.get_attribute("face_normal");
    assert(normals.size() == num_faces * 3);

    VectorF& frame_field = m_values;
    frame_field = VectorF::Zero(num_faces * 6); // 2x3 matrix for each face.

    for (size_t i=0; i<num_faces; i++) {
        VectorF edges = edge_lengths.segment(
                i*num_vertex_per_face, num_vertex_per_face);
        Vector3F n = normals.segment<3>(i*3);
        VectorF::Index max_idx;
        edges.maxCoeff(&max_idx);
        const Vector3F v0 = mesh.get_vertex(
                faces[i*num_vertex_per_face + max_idx]);
        const Vector3F v1 = mesh.get_vertex(
                faces[i*num_vertex_per_face + (max_idx+1) % num_vertex_per_face]);
        Vector3F e0 = v1 - v0;
        e0.normalize();
        Vector3F e1 = n.cross(e0);
        frame_field.segment<6>(i*6) << e0, e1;
    }
}
