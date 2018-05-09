/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "AABBTree.h"
#include <Geogram/GeogramMeshUtils.h>

using namespace PyMesh;

void PyMesh::Geogram::AABBTree::build() {
    constexpr bool REORDER = true; // Reorder seems to influence the results.

    m_geo_mesh = GeogramMeshUtils::form_mesh(m_vertices, m_faces);
    m_tree = std::make_shared<Tree>(*m_geo_mesh, REORDER);
}

void PyMesh::Geogram::AABBTree::lookup(const MatrixFr& points,
        VectorF& squared_distances,
        VectorI& closest_faces,
        MatrixFr& closest_points) const {
    const size_t dim = m_vertices.cols();
    assert(dim == points.cols());
    const size_t num_pts = points.rows();
    squared_distances.resize(num_pts);
    closest_faces.resize(num_pts);
    closest_points.resize(num_pts, dim);

    for (size_t i=0; i<num_pts; i++) {
        GEO::vec3 p(points(i,0), points(i,1), points(i,2));
        GEO::vec3 p2;
        closest_faces[i] = m_tree->nearest_facet(p, p2, squared_distances[i]);
        closest_points.row(i) << p2[0], p2[1], p2[2];
    }
}
