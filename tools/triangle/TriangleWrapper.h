/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <list>
#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class TriangleWrapper {
    public:
        TriangleWrapper(const MatrixFr& points, const MatrixIr& segments) :
            m_points(points), m_segments(segments), m_max_area(0.0) {}

    public:
        typedef std::list<size_t> Region;

    public:
        /**
         * Set hole points.  Used by triangle to flood and remove faces
         * representing holes.
         *
         * One can get by without setting holes by setting the
         * auto_hole_detection flag in run(...).
         */
        void set_holes(const MatrixFr& holes) { m_holes = holes; }

        /**
         * Depending on the column of segments, either triangulate a PSLG or
         * refine an existing triangulation.
         */
        void run(Float max_area,
                bool split_boundary=true,
                bool auto_hole_detection=false,
                bool use_steiner_points=true);

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }

    protected:
        void process_2D_input(const std::string& flags, bool auto_hole_detection);

        void run_triangle(
                const MatrixFr& points,
                const MatrixIr& segments,
                const MatrixFr& holes,
                const std::string& flags);
        void refine(
                const MatrixFr& points,
                const MatrixIr& faces,
                const std::string& flags);
        void poke_holes();
        bool select_seed_point(const Region& region, VectorF& seed_p);
        void correct_orientation();

    private:
        // Input data
        MatrixFr m_points;
        MatrixFr m_holes;
        MatrixIr m_segments;
        Float    m_max_area;

        // Output data
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_face_neighbors;
        VectorI  m_edges;
        VectorI  m_edge_marks;
};

}
