/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "GeogramMeshUtils.h"
#include <Core/Exception.h>
#include "GeogramBase.h"

using namespace PyMesh;

GeogramMeshUtils::GeoMeshPtr GeogramMeshUtils::form_mesh(const Mesh::Ptr mesh) {
    const size_t dim = mesh->get_dim();
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const auto& vertices = mesh->get_vertices();
    const auto& faces = mesh->get_faces();

    if (vertex_per_face != 3) {
        throw NotImplementedError("Converting non-triangle mesh to "
                "Geogram mesh is not yet implemented");
    }

    auto geo_mesh = std::make_shared<GeoMesh>(dim, false);
    geo_mesh->vertices.clear();
    geo_mesh->vertices.create_vertices(num_vertices);
    geo_mesh->facets.clear();
    geo_mesh->facets.create_triangles(num_faces);

    for (size_t i=0; i<num_vertices; i++) {
        auto& p = geo_mesh->vertices.point(i);
        for (size_t j=0; j<dim; j++) {
            p[j] = vertices[i*dim+j];
        }
    }

    for (size_t i=0; i<num_faces; i++) {
        geo_mesh->facets.set_vertex(i, 0, faces[i*3]);
        geo_mesh->facets.set_vertex(i, 1, faces[i*3+1]);
        geo_mesh->facets.set_vertex(i, 2, faces[i*3+2]);
    }

    return geo_mesh;
}

GeogramMeshUtils::GeoMeshPtr GeogramMeshUtils::form_mesh(
        const MatrixFr& vertices, const MatrixIr& faces) {
    const size_t dim = vertices.cols();
    const size_t vertex_per_face = faces.cols();
    const size_t num_vertices = vertices.rows();
    const size_t num_faces = faces.rows();

    if (vertex_per_face != 3) {
        throw NotImplementedError("Converting non-triangle mesh to "
                "Geogram mesh is not yet implemented");
    }

    auto geo_mesh = std::make_shared<GeoMesh>(dim, false);
    geo_mesh->vertices.clear();
    geo_mesh->vertices.create_vertices(num_vertices);
    geo_mesh->facets.clear();
    geo_mesh->facets.create_triangles(num_faces);

    for (size_t i=0; i<num_vertices; i++) {
        auto& p = geo_mesh->vertices.point(i);
        for (size_t j=0; j<dim; j++) {
            p[j] = vertices(i,j);
        }
    }

    for (size_t i=0; i<num_faces; i++) {
        geo_mesh->facets.set_vertex(i, 0, faces(i,0));
        geo_mesh->facets.set_vertex(i, 1, faces(i,1));
        geo_mesh->facets.set_vertex(i, 2, faces(i,2));
    }

    return geo_mesh;
}

