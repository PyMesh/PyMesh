/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>
#include <Misc/TripletMap.h>

namespace PyMesh {

class DegeneratedTriangleRemoval {
    public:
        DegeneratedTriangleRemoval(
                const MatrixFr& vertices, const MatrixIr& faces);

    public:
        void run(size_t num_iteraitons=5);

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_ori_face_indices() const { return m_ori_face_indices; }

    private:
        void init_ori_face_indices();
        size_t remove_zero_edges();
        size_t remove_line_faces();
        void remove_isolated_vertices();
        void init_edge_map();
        bool is_degenerated(size_t i) const;
        size_t find_longest_edge(size_t fi) const;

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        TripletMap<size_t> m_edge_map;
        VectorI m_ori_face_indices;
};

}
