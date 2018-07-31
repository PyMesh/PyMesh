/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#ifdef WITH_TRIANGLE

#include "ShewchukTriangle.h"
#include <Core/Exception.h>
#include <Triangle/TriangleWrapper.h>

using namespace PyMesh;

void ShewchukTriangle::run() {
    const size_t num_points = m_vertices.rows();
    const size_t dim = m_vertices.cols();
    if (dim != 2) {
        throw RuntimeError("Triangle only works in 2D domain.");
    }

    TriangleWrapper triangle;
    triangle.set_points(m_vertices);
    triangle.set_verbosity(0);
    triangle.set_max_num_steiner_points(0);
    triangle.set_min_angle(0);
    triangle.run();
    m_faces = triangle.get_faces();
}

#endif
