/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "FaceRadiusEdgeRatioAttribute.h"

#include <limits>
#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void FaceRadiusEdgeRatioAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();
    if (num_faces > 0 && vertex_per_face != 3) {
        throw NotImplementedError(
                "Face radius/edge ratio computation only support triangle faces.");
    }

    if (!mesh.has_attribute("face_circumradius")) {
        mesh.add_attribute("face_circumradius");
    }
    const auto& circum_radii = mesh.get_attribute("face_circumradius");

    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    const auto& lengths = mesh.get_attribute("edge_length");

    VectorF& re_ratio = m_values;
    re_ratio.resize(num_faces);

    for (size_t i=0; i<num_faces; i++) {
        const auto min_edge = lengths.segment<3>(i*3).minCoeff();
        const auto radius = circum_radii[i];
        if (min_edge == 0.0) {
            re_ratio[i] = std::numeric_limits<Float>::infinity();
        } else {
            re_ratio[i] = radius / min_edge;
        }
    }
}
