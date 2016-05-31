/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FaceNormalAttribute.h"

#include <sstream>

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void FaceNormalAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& normals = m_values;
    normals = VectorF::Zero(num_faces * 3); // Face normal is always in 3D

    if (dim == 3 || dim == 2) {
        if (num_vertex_per_face == 3) {
            for (size_t i=0; i<num_faces; i++) {
                normals.segment<3>(i*3) = compute_triangle_normal(mesh, i);
            }
        } else if (num_vertex_per_face == 4) {
            for (size_t i=0; i<num_faces; i++) {
                normals.segment<3>(i*3) = compute_quad_normal(mesh, i);
            }
        } else {
            std::stringstream err_msg;
            err_msg << "Normal computation of face with "
                << num_vertex_per_face << " is not supported.";
            throw NotImplementedError(err_msg.str());
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Dimension " << dim << " is not supported.";
        throw RuntimeError(err_msg.str());
    }
}

Vector3F FaceNormalAttribute::compute_triangle_normal(Mesh& mesh, size_t i) {
    const size_t dim = mesh.get_dim();
    VectorI face = mesh.get_face(i);
    assert(face.size() == 3);

    Vector3F v[3] = {
        Vector3F::Zero(),
        Vector3F::Zero(),
        Vector3F::Zero()
    };

    v[0].segment(0, dim) = mesh.get_vertex(face[0]);
    v[1].segment(0, dim) = mesh.get_vertex(face[1]);
    v[2].segment(0, dim) = mesh.get_vertex(face[2]);

    Vector3F normal = (v[1] - v[0]).cross(v[2] - v[0]);
    normal.normalize();
    return normal;
}

Vector3F FaceNormalAttribute::compute_quad_normal(Mesh& mesh, size_t i) {
    const size_t dim = mesh.get_dim();
    VectorI face = mesh.get_face(i);
    assert(face.size() == 4);

    Vector3F v[4] = {
        Vector3F::Zero(),
        Vector3F::Zero(),
        Vector3F::Zero(),
        Vector3F::Zero()
    };

    v[0].segment(0, dim) = mesh.get_vertex(face[0]);
    v[1].segment(0, dim) = mesh.get_vertex(face[1]);
    v[2].segment(0, dim) = mesh.get_vertex(face[2]);
    v[3].segment(0, dim) = mesh.get_vertex(face[3]);

    Vector3F normal = (v[2] - v[0]).cross(v[3] - v[1]);
    normal.normalize();
    return normal;
}

