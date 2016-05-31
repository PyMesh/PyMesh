/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexLaplacianAttribute.h"
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void VertexLaplacianAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (vertex_per_face != 3) {
        throw NotImplementedError(
                "Only triangle Laplacian is supported for now.");
    }

    VectorF& laplacian = m_values;
    laplacian = VectorF::Zero(num_vertices * dim);
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        VectorF v0 = mesh.get_vertex(face[0]);
        VectorF v1 = mesh.get_vertex(face[1]);
        VectorF v2 = mesh.get_vertex(face[2]);
        assert(face.size() == vertex_per_face);
        VectorF cotan_weights = compute_cotan_weights(v0, v1, v2);
        laplacian.segment(dim*face[0], dim) += cotan_weights[2] * (v0-v1) + cotan_weights[1] * (v0-v2);
        laplacian.segment(dim*face[1], dim) += cotan_weights[0] * (v1-v2) + cotan_weights[2] * (v1-v0);
        laplacian.segment(dim*face[2], dim) += cotan_weights[1] * (v2-v0) + cotan_weights[0] * (v2-v1);
    }
}

VectorF VertexLaplacianAttribute::compute_cotan_weights(
        const VectorF& v0, const VectorF& v1, const VectorF& v2) {
    size_t dim = v0.size();
    Vector3F e0(0,0,0);
    Vector3F e1(0,0,0);
    Vector3F e2(0,0,0);
    e0.segment(0,dim) = v2 - v1;
    e1.segment(0,dim) = v0 - v2;
    e2.segment(0,dim) = v1 - v0;
    Vector3F cotan_weights(
            (e2.dot(-e1) / (e2.cross(-e1)).norm()),
            (e0.dot(-e2) / (e0.cross(-e2)).norm()),
            (e1.dot(-e0) / (e1.cross(-e0)).norm()));
    return cotan_weights * 0.5;
}
