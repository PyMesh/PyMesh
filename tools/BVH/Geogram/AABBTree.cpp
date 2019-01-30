/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_GEOGRAM

#include "AABBTree.h"
#include <Geogram/GeogramMeshUtils.h>

using namespace PyMesh;

void PyMesh::Geogram::AABBTree::build() {
    constexpr bool REORDER = true; // Reorder seems to influence the results.
    const size_t num_faces = m_faces.rows();

    m_geo_mesh = GeogramMeshUtils::raw_to_geomesh(m_vertices, m_faces);
    GEO::Attribute<int> attr(m_geo_mesh->facets.attributes(), "id");
    for(int i=0; i < num_faces; i++) {
        attr[i] = i;
    }
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

    GEO::Attribute<int> ori_fid (m_geo_mesh->facets.attributes(), "id");

    for (size_t i=0; i<num_pts; i++) {
        GEO::vec3 p(points(i,0), points(i,1), points(i,2));
        GEO::vec3 p2;
        auto reordered_fid = m_tree->nearest_facet(p, p2, squared_distances[i]);
        closest_faces[i] = ori_fid[reordered_fid];
        closest_points.row(i) << p2[0], p2[1], p2[2];
    }
}

#endif
