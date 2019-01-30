/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_TRIANGLE

#include "ConstrainedDelaunay.h"
#include <Core/Exception.h>
#include <Triangle/TriangleWrapper.h>

using namespace PyMesh;

void Triangle::ConstrainedDelaunay::run() {
    TriangleWrapper triangle;
    triangle.set_points(m_points);
    triangle.set_segments(m_segments);
    triangle.set_split_boundary(false);
    triangle.set_max_num_steiner_points(0);
    triangle.keep_convex_hull(true);
    triangle.set_min_angle(0);
    triangle.set_verbosity(0);
    triangle.run();
    m_vertices = triangle.get_vertices();
    auto faces = triangle.get_faces();
    if (faces.rows() > 0) {
        m_faces = triangle.get_faces();
    }
}

#endif
