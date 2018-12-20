/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#ifdef WITH_TRIANGLE

#include "ConformingDelaunay.h"
#include <Core/Exception.h>
#include <Triangle/TriangleWrapper.h>

using namespace PyMesh;

void Triangle::ConformingDelaunay::run() {
    TriangleWrapper triangle;
    triangle.set_points(m_points);
    triangle.set_segments(m_segments);
    triangle.set_conforming_delaunay(true);
    triangle.keep_convex_hull(true);
    //triangle.set_min_angle(0.0);
    triangle.set_verbosity(0);
    triangle.run();
    m_vertices = triangle.get_vertices();
    auto faces = triangle.get_faces();
    if (faces.rows() > 0) {
        m_faces = triangle.get_faces();
    }
}

#endif
