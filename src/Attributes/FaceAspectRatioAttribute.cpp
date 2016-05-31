/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */

#include "FaceAspectRatioAttribute.h"

#include <iostream>
#include <limits>

#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void FaceAspectRatioAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();
    if (num_vertex_per_face != 3 && num_vertex_per_face != 4) {
        throw NotImplementedError(
                "Only triangle and quad aspect ratio is supported");
    }

    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    const VectorF& edge_length = mesh.get_attribute("edge_length");

    const size_t num_faces = mesh.get_num_faces();

    VectorF& aspect_ratios = m_values;
    aspect_ratios = VectorF::Zero(num_faces);

    if (num_vertex_per_face == 3) {
        // For triangle, aspect ratio is the ratio of circumradius to twice the
        // incircle radius.
        for (size_t i=0; i<num_faces; i++) {
            const Float a = edge_length[i*3+1];
            const Float b = edge_length[i*3+2];
            const Float c = edge_length[i*3+0];
            const Float s = (a+b+c) / 2.0;

            if (s == a+b || s == b+c || s == c+a) {
                aspect_ratios[i] = std::numeric_limits<Float>::infinity();
            } else {
                aspect_ratios[i] = a*b*c/(8*(a+b-s)*(b+c-s)*(c+a-s));
            }
        }
    } else {
        // For quad, aspect ratio is the ratio of the longest edge to the
        // shortest edge.
        for (size_t i=0; i<num_faces; i++) {
            VectorF side_lengths = edge_length.segment(
                    i*num_vertex_per_face, num_vertex_per_face);
            const Float min_edge = side_lengths.minCoeff();
            const Float max_edge = side_lengths.maxCoeff();
            if (min_edge == 0.0) {
                aspect_ratios[i] = std::numeric_limits<Float>::infinity();
            } else {
                aspect_ratios[i] = max_edge / min_edge;
            }
        }
    }

    if (!aspect_ratios.allFinite()) {
        std::cerr << "Warning: degenerate triangle has infinite aspect ratio"
            << std::endl;
    }
}
