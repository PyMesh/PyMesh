/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexMeanCurvatureAttribute.h"
#include <cassert>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void VertexMeanCurvatureAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    VectorF laplacian = compute_laplacian_vectors(mesh);
    VectorF normals = compute_vertex_normals(mesh);
    assert(laplacian.size() == dim*num_vertices);
    assert(normals.size() == dim*num_vertices);

    if (!mesh.has_attribute("vertex_voronoi_area")) {
        mesh.add_attribute("vertex_voronoi_area");
    }

    const auto& area = mesh.get_attribute("vertex_voronoi_area");
    VectorF& mean_curvature = m_values;
    mean_curvature = VectorF::Zero(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        mean_curvature[i] = laplacian.segment(dim*i,dim).norm() * 0.5;
        Float sign = laplacian.segment(dim*i, dim).dot(normals.segment(dim*i,dim));
        if (sign < 0) {
            mean_curvature[i] *= -1;
        }
    }
    mean_curvature = mean_curvature.array() / area.array();
}

VectorF VertexMeanCurvatureAttribute::compute_laplacian_vectors(Mesh& mesh) {
    if (!mesh.has_attribute("vertex_laplacian")) {
        mesh.add_attribute("vertex_laplacian");
    }
    return mesh.get_attribute("vertex_laplacian");
}

VectorF VertexMeanCurvatureAttribute::compute_vertex_normals(Mesh& mesh) {
    if (!mesh.has_attribute("vertex_normal")) {
        mesh.add_attribute("vertex_normal");
    }
    return mesh.get_attribute("vertex_normal");
}
