/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FaceVoronoiAreaAttribute.h"

#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void FaceVoronoiAreaAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_faces = mesh.get_num_faces();
    size_t vertex_per_face = mesh.get_vertex_per_face();

    if (vertex_per_face != 3) {
        throw NotImplementedError("Only triangle face are supported currently.");
    }

    VectorF& voronoi_areas = m_values;
    voronoi_areas = VectorF::Zero(num_faces * vertex_per_face);

    for (size_t i=0; i<num_faces; i++) {
        VectorF areas = compute_triangle_voronoi_area(mesh, i);
        voronoi_areas.segment(i*vertex_per_face, vertex_per_face) = areas;
    }
}

VectorF FaceVoronoiAreaAttribute::compute_triangle_voronoi_area(
        Mesh& mesh, size_t face_idx) {
    size_t dim = mesh.get_dim();
    VectorI face = mesh.get_face(face_idx);

    Vector3F v0 = Vector3F::Zero();
    Vector3F v1 = Vector3F::Zero();
    Vector3F v2 = Vector3F::Zero();

    v0.segment(0, dim) = mesh.get_vertex(face[0]);
    v1.segment(0, dim) = mesh.get_vertex(face[1]);
    v2.segment(0, dim) = mesh.get_vertex(face[2]);

    Vector3F e0 = v2 - v1;
    Vector3F e1 = v0 - v2;
    Vector3F e2 = v1 - v0;

    Float double_area = e2.cross(-e1).norm();

    Float cot_0 = e2.dot(-e1) / e2.cross(-e1).norm();
    Float cot_1 = e0.dot(-e2) / e0.cross(-e2).norm();
    Float cot_2 = e1.dot(-e0) / e1.cross(-e0).norm();

    // Handle obtuse triangles.
    if (cot_0 < 0.0) {
        return Vector3F(0.5, 0.25, 0.25) * double_area * 0.5;
    } else if (cot_1 < 0.0) {
        return Vector3F(0.25, 0.5, 0.25) * double_area * 0.5;
    } else if (cot_2 < 0.0) {
        return Vector3F(0.25, 0.25, 0.5) * double_area * 0.5;
    }

    Float sq_l0 = e0.squaredNorm();
    Float sq_l1 = e1.squaredNorm();
    Float sq_l2 = e2.squaredNorm();

    return Vector3F(
            sq_l1 * cot_1 + sq_l2 * cot_2,
            sq_l0 * cot_0 + sq_l2 * cot_2,
            sq_l1 * cot_1 + sq_l0 * cot_0) / 8.0;
}
