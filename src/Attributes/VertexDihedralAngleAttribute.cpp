/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexDihedralAngleAttribute.h"

#include <Mesh.h>

using namespace PyMesh;

void VertexDihedralAngleAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (!mesh.has_attribute("edge_dihedral_angle")) {
        mesh.add_attribute("edge_dihedral_angle");
    }
    auto& edge_dihedral_angles = mesh.get_attribute("edge_dihedral_angle");

    auto& vertex_dihedral_angles = m_values;
    vertex_dihedral_angles = VectorF::Zero(num_vertices);
    for (size_t i=0; i<num_faces; i++) {
        const auto f = mesh.get_face(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            Float& cur_v = vertex_dihedral_angles[f[j]];
            cur_v = std::max(cur_v, edge_dihedral_angles[
                    i*vertex_per_face+j ]);
            cur_v = std::max(cur_v, edge_dihedral_angles[
                    i*vertex_per_face+(j-1+vertex_per_face)%vertex_per_face ]);
        }
    }
}
