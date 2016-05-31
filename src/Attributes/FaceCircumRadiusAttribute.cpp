#include "FaceCircumRadiusAttribute.h"

#include <iostream>
#include <limits>

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void FaceCircumRadiusAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (vertex_per_face != 3) {
        throw RuntimeError("Circumradius is only defined for triangle faces.");
    }

    if (!mesh.has_attribute("edge_length")) {
        mesh.add_attribute("edge_length");
    }
    if (!mesh.has_attribute("face_area")) {
        mesh.add_attribute("face_area");
    }
    VectorF edge_length = mesh.get_attribute("edge_length");
    VectorF face_area = mesh.get_attribute("face_area");

    VectorF& circumradii = m_values;
    circumradii.resize(num_faces);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        if (face_area[i] == 0.0) {
            circumradii[i] = std::numeric_limits<Float>::infinity();
        } else {
            circumradii[i] = (edge_length[3*i] * edge_length[3*i+1] *
                    edge_length[3*i+2]) / (4*face_area[i]);
        }
    }

    if (!circumradii.allFinite()) {
        std::cerr << "Warning: "
            << "circumradii is not all finite due to degenearte triangles"
            << std::endl;
    }
}

