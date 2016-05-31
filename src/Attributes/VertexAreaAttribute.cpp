/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexAreaAttribute.h"

#include <Mesh.h>

using namespace PyMesh;

void VertexAreaAttribute::compute_from_mesh(Mesh& mesh) {
    size_t num_faces = mesh.get_num_faces();
    size_t num_vertices = mesh.get_num_vertices();
    size_t num_vertex_per_face = mesh.get_vertex_per_face();
    VectorF& areas = get_face_areas(mesh);

    VectorF& vertex_area = m_values;
    vertex_area = VectorF::Zero(num_vertices);

    for (size_t i=0; i<num_faces; i++) {
        Float per_vertex_area = areas[i] / num_vertex_per_face;
        VectorI face = mesh.get_face(i);
        assert(face.size() == num_vertex_per_face);
        for (size_t j=0; j<num_vertex_per_face; j++) {
            vertex_area[face[j]] += per_vertex_area;
        }
    }
}

VectorF& VertexAreaAttribute::get_face_areas(Mesh& mesh) {
    const std::string face_area_attribute = "face_area";
    if (!mesh.has_attribute(face_area_attribute)) {
        mesh.add_attribute(face_area_attribute);
    }
    return mesh.get_attribute(face_area_attribute);
}
