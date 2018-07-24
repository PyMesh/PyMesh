/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "AABBTree.h"

using namespace PyMesh;

void PyMesh::IGL::AABBTree::build() {
    m_tree.init(m_vertices, m_faces);
}

void PyMesh::IGL::AABBTree::lookup(const MatrixFr& points,
        VectorF& squared_distances,
        VectorI& closest_faces,
        MatrixFr& closest_points) const {
    m_tree.squared_distance(m_vertices, m_faces, points,
            squared_distances, closest_faces, closest_points);
}
