/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FaceCentroidAttribute.h"

#include <Mesh.h>

using namespace PyMesh;

void FaceCentroidAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    VectorF& centroids = m_values;
    centroids.resize(num_faces * dim);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        VectorF centroid = VectorF::Zero(dim);
        for (size_t j=0; j<vertex_per_face; j++) {
            centroid += mesh.get_vertex(face[j]);
        }
        centroid /= vertex_per_face;

        centroids.segment(i*dim, dim) = centroid;
    }
}
