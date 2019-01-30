/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#ifdef WITH_TRIANGLE
#include "TriangleConvexHullEngine.h"

#include <cassert>
#include <Core/Exception.h>
#include <Triangle/TriangleWrapper.h>
#include <MeshUtils/Boundary.h>

using namespace PyMesh;

void TriangleConvexHullEngine::run(const MatrixFr& points) {
    TriangleWrapper triangle;
    triangle.keep_convex_hull(true);
    triangle.set_verbosity(0);
    triangle.set_points(points);
    triangle.run();

    auto vertices = triangle.get_vertices();
    auto faces = triangle.get_faces();
    auto bd_extractor = Boundary::extract_surface_boundary_raw(vertices, faces);

    // Extract boundary.
    const size_t num_boundary_nodes = bd_extractor->get_num_boundary_nodes();
    m_index_map = bd_extractor->get_boundary_nodes();
    assert(m_index_map.size() == num_boundary_nodes);
    m_vertices.resize(num_boundary_nodes, vertices.cols());
    VectorI vertex_map(vertices.rows());
    vertex_map.setConstant(-1);
    for (size_t i=0; i<num_boundary_nodes; i++) {
        m_vertices.row(i) = vertices.row(m_index_map[i]);
        vertex_map[m_index_map[i]] = i;
    }

    const size_t num_boundaries = bd_extractor->get_num_boundaries();
    m_faces = bd_extractor->get_boundaries();
    assert(m_faces.rows() == num_boundaries);
    assert(m_faces.cols() == 2);

    for (size_t i=0; i<num_boundaries; i++) {
        m_faces(i, 0) = vertex_map[m_faces(i,0)];
        m_faces(i, 1) = vertex_map[m_faces(i,1)];
    }
}

#endif
