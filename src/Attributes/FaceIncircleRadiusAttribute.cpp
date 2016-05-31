/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include "FaceIncircleRadiusAttribute.h"

#include <Mesh.h>
#include <Core/Exception.h>
#include <iostream>

using namespace PyMesh;

void FaceIncircleRadiusAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();
    if (num_vertex_per_face != 3) {
        throw NotImplementedError("Incircle only exists for triangles.");
    }

    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    if (!mesh.has_attribute("face_area")) {
        mesh.add_attribute("face_area");
    }

    const VectorF& edge_length = mesh.get_attribute("edge_length");
    const VectorF& areas = mesh.get_attribute("face_area");

    VectorF& radii = m_values;
    radii = VectorF::Zero(num_faces);

    for (size_t i=0; i<num_faces; i++) {
        Float circumference = edge_length.segment(
                i*num_vertex_per_face, num_vertex_per_face).sum();
        Float area = areas[i];
        if (circumference != 0)
            radii[i] = 2.0 * area / circumference;
        else
            radii[i] = 0.0;
    }
}

