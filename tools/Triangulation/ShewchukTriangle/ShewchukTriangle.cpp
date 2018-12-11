/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#ifdef WITH_TRIANGLE

#include "ShewchukTriangle.h"
#include <Core/Exception.h>
#include <Triangle/TriangleWrapper.h>

using namespace PyMesh;

void ShewchukTriangle::run() {
    const size_t num_points = m_points.rows();

    TriangleWrapper triangle;
    triangle.set_points(m_points);
    triangle.set_segments(m_segments);
    triangle.set_verbosity(0);
    triangle.run();
    m_vertices = triangle.get_vertices();
    auto faces = triangle.get_faces();
    if (faces.rows() > 0) {
        m_faces = triangle.get_faces();
    }
}

#endif
