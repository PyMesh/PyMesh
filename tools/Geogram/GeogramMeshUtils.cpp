/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_GEOGRAM

#include "GeogramMeshUtils.h"
#include <Core/Exception.h>
#include <MeshFactory.h>
#include "GeogramBase.h"

using namespace PyMesh;

GeoMeshPtr GeogramMeshUtils::mesh_to_geomesh(const Mesh::Ptr mesh) {
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

GeoMeshPtr GeogramMeshUtils::raw_to_geomesh(
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

GeoMeshPtr GeogramMeshUtils::wire_network_to_geomesh(
        const MatrixFr& vertices, const Matrix2Ir& edges) {
    const size_t dim = vertices.cols();
    const size_t num_vertices = vertices.rows();
    const size_t num_edges = edges.rows();

    auto geo_mesh = std::make_shared<GeoMesh>(dim, false);
    geo_mesh->vertices.clear();
    geo_mesh->vertices.create_vertices(num_vertices);
    geo_mesh->edges.clear();
    geo_mesh->edges.create_edges(num_edges);

    for (size_t i=0; i<num_vertices; i++) {
        auto& p = geo_mesh->vertices.point(i);
        for (size_t j=0; j<dim; j++) {
            p[j] = vertices(i,j);
        }
    }

    for (size_t i=0; i<num_edges; i++) {
        geo_mesh->edges.set_vertex(i, 0, edges(i,0));
        geo_mesh->edges.set_vertex(i, 1, edges(i,1));
    }

    return geo_mesh;
}

Mesh::Ptr GeogramMeshUtils::geomesh_to_mesh(const GeoMeshPtr geo_mesh) {
    constexpr size_t dim = 3;
    const size_t num_vertices = geo_mesh->vertices.nb();
    const size_t num_faces = geo_mesh->facets.nb();
    const size_t num_voxels = geo_mesh->cells.nb();

    const size_t vertex_per_face =
        num_faces > 0 ? geo_mesh->facets.nb_vertices(0) : 3;
    const size_t vertex_per_voxel =
        num_voxels > 0 ? geo_mesh->cells.nb_vertices(0) : 4;

    VectorF vertices(num_vertices * dim);
    VectorI faces(num_faces * vertex_per_face);
    VectorI voxels(num_voxels * vertex_per_voxel);

    for (size_t i=0; i<num_vertices; i++) {
        const auto& v = geo_mesh->vertices.point(i);
        vertices[i*3]   = v[0];
        vertices[i*3+1] = v[1];
        vertices[i*3+2] = v[2];
    }
    for (size_t i=0; i<num_faces; i++) {
        if (geo_mesh->facets.nb_vertices(i) != vertex_per_face) {
            throw NotImplementedError("Mixed faces detected in Geogram."
                    " It is not supported by Pymesh");
        }
        for (size_t j=0; j<vertex_per_face; j++) {
            faces[i*vertex_per_face+j] = geo_mesh->facets.vertex(i, j);
        }
    }
    for (size_t i=0; i<num_voxels; i++) {
        if (geo_mesh->cells.nb_vertices(i) != vertex_per_voxel) {
            throw NotImplementedError("Mixed cells detected in Geogram."
                    " It is not supported by Pymesh");
        }
        for (size_t j=0; j<vertex_per_voxel; j++) {
            voxels[i*vertex_per_voxel+j] = geo_mesh->cells.vertex(i, j);
        }
    }

    return MeshFactory().load_data(vertices, faces, voxels,
                dim, vertex_per_face, vertex_per_voxel).create();
}

#endif
