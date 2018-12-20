/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <Core/EigenTypedef.h>

namespace PyMesh {

/**
 * Compute 2D snap rounding.
 *
 * Note that the output graph may contain exactly duplicate vertices.
 */
class SnapRounding2 {
    public:
        SnapRounding2() = default;

    public:
        void set_points(const Matrix2Fr& points) { m_points = points; }
        const Matrix2Fr& get_points() const { return m_points; }

        void set_segments(const Matrix2Ir& segments) { m_segments = segments; }
        const Matrix2Ir& get_segments() const { return m_segments; }

        void run(Float pixel_size, bool iterative);

        const Matrix2Fr get_vertices() const { return m_vertices; }
        const Matrix2Ir get_edges() const { return m_edges; }

    private:
        Matrix2Fr m_points;
        Matrix2Ir m_segments;
        Matrix2Fr m_vertices;
        Matrix2Ir m_edges;

};

}


#endif
