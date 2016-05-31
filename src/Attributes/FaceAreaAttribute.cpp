/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FaceAreaAttribute.h"

#include <cassert>
#include <iostream>

#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void FaceAreaAttribute::compute_from_mesh(Mesh& mesh) {
    size_t num_faces = mesh.get_num_faces();
    size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& areas = m_values;
    areas = VectorF::Zero(num_faces);

    if (num_vertex_per_face == 3) {
        for (size_t i=0; i<num_faces; i++) {
            areas[i] = compute_triangle_area(mesh, i);
        }
    } else if (num_vertex_per_face == 4) {
        for (size_t i=0; i<num_faces; i++) {
            areas[i] = compute_quad_area(mesh, i);
        }
    } else {
        std::cerr << "Face area attribute with " << num_vertex_per_face
            << " vertices per face is not supported yet." << std::endl;
        throw NotImplementedError("Only triangle faces are supported.");
        return;
    }
}

Float FaceAreaAttribute::compute_triangle_area(Mesh& mesh, size_t face_idx) {
    const size_t dim = mesh.get_dim();
    VectorI face = mesh.get_face(face_idx);
    assert(face.size() == 3);

    Vector3F v[3] = {
        Vector3F::Zero(),
        Vector3F::Zero(),
        Vector3F::Zero()
    };

    v[0].segment(0, dim) = mesh.get_vertex(face[0]);
    v[1].segment(0, dim) = mesh.get_vertex(face[1]);
    v[2].segment(0, dim) = mesh.get_vertex(face[2]);

    Float area = compute_triangle_area(v[0], v[1], v[2]);
    return area;
}

Float FaceAreaAttribute::compute_quad_area(Mesh& mesh, size_t face_idx) {
    const size_t dim = mesh.get_dim();
    VectorI face = mesh.get_face(face_idx);
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

    Vector3F centroid = (v[0] + v[1] + v[2] + v[3]) / 4;
    Float area =
        compute_triangle_area(v[0], v[1], centroid) +
        compute_triangle_area(v[1], v[2], centroid) +
        compute_triangle_area(v[2], v[3], centroid) +
        compute_triangle_area(v[3], v[0], centroid) ;
    return area;
}

