/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_CGAL

#include "AABBTree.h"

using namespace PyMesh;

void PyMesh::_CGAL::AABBTree::build() {
    m_triangles.clear();
    assert(m_faces.cols() == 3);

    const size_t num_faces = m_faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_faces.row(i);
        const auto& v0 = m_vertices.row(f[0]);
        const auto& v1 = m_vertices.row(f[1]);
        const auto& v2 = m_vertices.row(f[2]);
        m_triangles.emplace_back(
                Point(v0[0], v0[1], v0[2]),
                Point(v1[0], v1[1], v1[2]),
                Point(v2[0], v2[1], v2[2]));
    }

    m_tree = std::make_shared<Tree>(m_triangles.begin(), m_triangles.end());
    m_tree->accelerate_distance_queries();
}

void PyMesh::_CGAL::AABBTree::lookup(const MatrixFr& points,
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
        Point p(points(i,0), points(i,1), points(i,2));
        Point_and_primitive_id itr = m_tree->closest_point_and_primitive(p);
        closest_faces[i] = itr.second - m_triangles.begin();
        Point p2 = itr.first;
        closest_points.row(i) << p2[0], p2[1], p2[2];
        squared_distances[i] = (p-p2).squared_length();
    }
}

#endif
