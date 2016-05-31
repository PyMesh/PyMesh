/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FaceCircumCenterAttribute.h"

#include <Mesh.h>
#include <Core/Exception.h>
#include <iostream>
#include <limits>

using namespace PyMesh;

void FaceCircumCenterAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (vertex_per_face != 3) {
        throw RuntimeError("Circumcenter is only defined for triangle faces.");
    }

    if (!mesh.has_attribute("edge_squared_length")) {
        mesh.add_attribute("edge_squared_length");
    }
    VectorF edge_sq_length = mesh.get_attribute("edge_squared_length");

    VectorF& circum_centers = m_values;
    circum_centers.resize(num_faces * dim);

    const VectorF& vertices = mesh.get_vertices();

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);

        VectorF v0 = vertices.segment(face[0]*dim, dim);
        VectorF v1 = vertices.segment(face[1]*dim, dim);
        VectorF v2 = vertices.segment(face[2]*dim, dim);

        Float sq_l0 = edge_sq_length[i*3+1];
        Float sq_l1 = edge_sq_length[i*3+2];
        Float sq_l2 = edge_sq_length[i*3+0];

        Vector3F coeff(
                sq_l0 * (sq_l1 + sq_l2 - sq_l0),
                sq_l1 * (sq_l0 + sq_l2 - sq_l1),
                sq_l2 * (sq_l0 + sq_l1 - sq_l2));
        Float sum = coeff.sum();
        if (sum == 0.0) {
            coeff.setConstant(std::numeric_limits<Float>::infinity());
        } else {
            coeff /= coeff.sum();
        }

        circum_centers.segment(i*dim, dim) =
            v0 * coeff[0] +
            v1 * coeff[1] +
            v2 * coeff[2];
    }

    if (!circum_centers.allFinite()) {
        std::cerr << "Warning: "
            << "circumcenter is not all finite due to degenearte triangles"
            << std::endl;
    }
}
