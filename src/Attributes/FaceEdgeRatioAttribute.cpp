/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */

#include "FaceEdgeRatioAttribute.h"

#include <iostream>
#include <limits>

#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void FaceEdgeRatioAttribute::compute_from_mesh(Mesh& mesh) {
    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    const VectorF& edge_length = mesh.get_attribute("edge_length");

    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& edge_ratios = m_values;
    edge_ratios = VectorF::Zero(num_faces);

    for (size_t i=0; i<num_faces; i++) {
        VectorF edges = edge_length.segment(
                i*num_vertex_per_face, num_vertex_per_face);
        const Float min_e = edges.minCoeff();
        const Float max_e = edges.maxCoeff();
        if (min_e == 0.0) {
            edge_ratios[i] = std::numeric_limits<Float>::infinity();
        } else {
            edge_ratios[i] = max_e / min_e;
        }
    }

    if (!edge_ratios.allFinite()) {
        std::cerr << "Warning: some triangles have infinite edge ratio"
            << std::endl;
    }
}
