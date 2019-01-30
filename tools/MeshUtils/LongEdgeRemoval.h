/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>
#include <Core/EigenTypedef.h>
#include <Misc/MultipletMap.h>

namespace PyMesh {

class LongEdgeRemoval {
    public:
        LongEdgeRemoval(const MatrixFr& vertices, const MatrixIr& faces) :
            m_vertices(vertices), m_faces(faces) {}

    public:
        void run(Float max_length, bool recursive=true);

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }

        VectorI get_ori_faces() const { return m_ori_faces; }

    private:
        void init_edge_map();
        void split_long_edges(Float max_length);
        size_t retriangulate();
        void triangulate_chain(
                std::vector<VectorI>& faces,
                const std::vector<size_t>& chain,
                size_t v0_idx, size_t v1_idx, size_t v2_idx);
        std::vector<size_t> get_vertex_chain_around_triangle(
                size_t fi, size_t& v0_idx, size_t& v1_idx, size_t& v2_idx);

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_ori_faces;

        DupletMap<size_t> m_edge_map;
};

}
