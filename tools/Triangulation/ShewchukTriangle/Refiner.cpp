/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#ifdef WITH_TRIANGLE

#include "Refiner.h"
#include <Core/Exception.h>
#include <Triangle/TriangleWrapper.h>

using namespace PyMesh;

void Triangle::Refiner::refine(const MatrixFr& metric) {
    TriangleWrapper triangle;
    triangle.set_points(m_vertices);
    triangle.set_triangles(m_faces);
    triangle.set_split_boundary(true);
    triangle.keep_convex_hull(false);
    triangle.set_verbosity(0);
    triangle.run();
    m_vertices = triangle.get_vertices();
    auto faces = triangle.get_faces();
    if (faces.rows() > 0) {
        m_faces = triangle.get_faces();
    }
}

#endif
