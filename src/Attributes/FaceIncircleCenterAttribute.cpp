/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include "FaceIncircleCenterAttribute.h"

#include <Mesh.h>
#include <Core/Exception.h>
#include <iostream>

using namespace PyMesh;

void FaceIncircleCenterAttribute::compute_from_mesh(Mesh& mesh) {
    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    const VectorF& edge_length = mesh.get_attribute("edge_length");

    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();

    if (num_vertex_per_face != 3) {
        throw NotImplementedError("Incircle only exists for triangles.");
    }

    VectorF& centers = m_values;
    centers = VectorF::Zero(num_faces*dim);

    const VectorF& vertices = mesh.get_vertices();
    const VectorI& faces = mesh.get_faces();

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = faces.segment(
                i*num_vertex_per_face, num_vertex_per_face);
        VectorF edges = edge_length.segment(
                i*num_vertex_per_face, num_vertex_per_face);

        Float circumference = edges.sum();
        if (circumference == 0) {
            // Triangle is so degenerate that it is a point.
            // The incenter would be that point.
            centers.segment(i*dim, dim) = vertices.segment(face[0]*dim, dim);
        } else {
            centers.segment(i*dim, dim) = (
                    vertices.segment(face[0]*dim, dim) * edges[1] +
                    vertices.segment(face[1]*dim, dim) * edges[2] +
                    vertices.segment(face[2]*dim, dim) * edges[0])
                / circumference;
        }
    }
}

