#pragma once
#include <string>
#include <Core/EigenTypedef.h>

class TriangleWrapper {
    public:
        TriangleWrapper(const MatrixFr& points, const MatrixIr& segments) : 
            m_points(points), m_segments(segments) {}

    public:
        void set_holes(const MatrixFr& holes) { m_holes = holes; }
        void run(Float max_area, bool split_boundary=true);

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }

    protected:
        void run_triangle(
                const MatrixFr& points,
                const MatrixIr& segments,
                const MatrixFr& holes,
                const std::string& flags);
        void refine(
                const MatrixFr& points,
                const MatrixIr& faces,
                const std::string& flags);

    private:
        // Input data
        MatrixFr m_points;
        MatrixFr m_holes;
        MatrixIr m_segments;

        // Output data
        MatrixFr m_vertices;
        MatrixIr m_faces;
};
