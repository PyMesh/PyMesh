/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexGaussianCurvatureAttribute.h"
#include <cmath>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void VertexGaussianCurvatureAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    VectorF& gaussian_curvature = m_values;
    gaussian_curvature = VectorF::Zero(num_vertices);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        VectorF angles = compute_face_angles(mesh, i);
        for (size_t j=0; j<vertex_per_face; j++) {
            gaussian_curvature[face[j]] += angles[j];
        }
    }
    gaussian_curvature = VectorF::Ones(num_vertices)*2*M_PI - gaussian_curvature;
}

VectorF VertexGaussianCurvatureAttribute::compute_face_angles(
        const Mesh& mesh, size_t face_idx) {
    const size_t dim = mesh.get_dim();
    const size_t vertex_per_face = mesh.get_vertex_per_face();
    VectorF angles = VectorF::Zero(vertex_per_face);
    VectorI face = mesh.get_face(face_idx);
    MatrixFr vertices = MatrixFr::Zero(vertex_per_face, 3);
    for (size_t i=0; i<vertex_per_face; i++) {
        vertices.row(i).segment(0, dim) = mesh.get_vertex(face[i]);
    }

    for (size_t i=0; i<vertex_per_face; i++) {
        size_t curr_idx = i;
        size_t next_idx = (i+1) % vertex_per_face;
        size_t prev_idx = (i-1+vertex_per_face) % vertex_per_face;
        Vector3F e1 = vertices.row(next_idx) - vertices.row(curr_idx);
        Vector3F e2 = vertices.row(prev_idx) - vertices.row(curr_idx);
        angles[i] = atan2(e1.cross(e2).norm(), e1.dot(e2));
    }

    return angles;
}
