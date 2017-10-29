/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "VertexVoronoiAreaAttribute.h"

#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void VertexVoronoiAreaAttribute::compute_from_mesh(Mesh& mesh) {
    if (!mesh.has_attribute("face_voronoi_area")) {
        mesh.add_attribute("face_voronoi_area");
    }

    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (vertex_per_face != 3) {
        throw NotImplementedError("Vertex Voronoi area computation only has triangle mesh support");
    }

    const auto& face_voronoi_areas = mesh.get_attribute("face_voronoi_area");
    auto& vertex_voronoi_areas = m_values;
    vertex_voronoi_areas = VectorF::Zero(num_vertices);
    const auto& faces = mesh.get_faces();

    for (size_t i=0; i<num_faces; i++) {
        const Vector3I f = faces.segment<3>(i*3);

        vertex_voronoi_areas[f[0]] += face_voronoi_areas[i*3];
        vertex_voronoi_areas[f[1]] += face_voronoi_areas[i*3+1];
        vertex_voronoi_areas[f[2]] += face_voronoi_areas[i*3+2];
    }
}
