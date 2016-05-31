/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>
#include <vector>
#include <map>

namespace PyMesh {

class PeriodicBoundaryRemesher {
    public:
        PeriodicBoundaryRemesher(
                const MatrixFr& vertices, const MatrixIr& faces, 
                const VectorF& bbox_min, const VectorF& bbox_max);

    public:
        void remesh(Float ave_edge_len);

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const {return m_faces; }

    protected:
        void clean_up();
        void label_bd_faces();
        void label_bd_faces(int axis);
        void extract_bd_loops();
        void extract_bd_loops(short label);
        void collapse_short_bd_edges(Float tol);
        void collapse_short_bd_edges(short label, Float tol);
        void match_bd_loops();
        void match_bd_loops(short axis);
        void collapse_unmatched_vertices(short label, std::vector<int>& vertex_map);
        void refine_bd_loops(Float ave_edge_len);
        void refine_bd_loops(short label, Float ave_edge_len,
                std::vector<Float>& refined_vertices);
        void remesh_boundary(Float max_area);

        void add_interior_geometry(
                std::vector<MatrixFr>& remeshed_vertices,
                std::vector<MatrixIr>& remeshed_faces,
                size_t& vertex_count, size_t& face_count);

        void triangulate_bd_loops(Float max_area, short axis,
                std::vector<MatrixFr>& remeshed_vertices,
                std::vector<MatrixIr>& remeshed_faces,
                size_t& vertex_count, size_t& face_count);

        void update_all_indices(const VectorI& vertex_map);

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;

        VectorF m_bbox_min;
        VectorF m_bbox_max;

        typedef std::vector<short> Markers;
        Markers m_bd_face_markers;

        // BoundaryLoops maps boundary label to an array of edges.
        typedef std::map<short, MatrixIr> BoundaryLoops;
        BoundaryLoops m_bd_loops;
};

}
