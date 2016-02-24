/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */

#include "FaceAspectRatioAttribute.h"

#include <limits>

#include <Core/Exception.h>
#include <Mesh.h>

void FaceAspectRatioAttribute::compute_from_mesh(Mesh& mesh) {
    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    const VectorF& edge_length = mesh.get_attribute("edge_length");

    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& aspect_ratios = m_values;
    aspect_ratios = VectorF::Zero(num_faces);
    const Float min_float = std::numeric_limits<Float>::min();
    const Float max_float = std::numeric_limits<Float>::max();
    const Float inf_float = std::numeric_limits<Float>::infinity();

    for (size_t i=0; i<num_faces; i++) {
        VectorF edges = edge_length.segment(
                i*num_vertex_per_face, num_vertex_per_face);
        const Float min_e = edges.minCoeff();
        const Float max_e = edges.maxCoeff();
        if (min_e <= min_float || max_e >= max_float) {
            aspect_ratios[i] = inf_float;
        } else {
            aspect_ratios[i] = max_e / min_e;
        }
    }
}
