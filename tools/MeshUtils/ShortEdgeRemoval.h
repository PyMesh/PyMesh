/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <vector>

#include <Core/EigenTypedef.h>
#include <Misc/Triplet.h>

#include "IndexHeap.h"

namespace PyMesh {

class ShortEdgeRemoval {
    public:
        ShortEdgeRemoval(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        /**
         * Importance is an integer value per vertex.  The vertex with higher
         * importance would keep its position during edge collapsing.  Mid-point
         * is used when collapsing edge with same importance at each end.
         *
         * Vertex with negative importance is not considered during collapsing.
         * i.e. they will keep their original location.
         */
        void set_importance(const VectorI& importance) {
            m_importance = importance;
        }
        /**
         * Remove all edges that <= thresold
         * If thresold=0, remove all degenerated edges.
         */
        size_t run(Float threshold);
        MatrixFr get_vertices() const;
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_face_indices() const { return m_face_indices; }

    private:
        typedef Triplet Edge;

        void init();
        void update();
        void init_vertex_map();
        void init_face_indices();
        void init_edges();
        void init_edge_length_heap();
        void init_vertex_face_neighbors();
        void update_vertices();
        void update_faces();
        void update_importance();
        void collapse(Float threshold);
        bool edge_is_valid(size_t edge_idx) const;
        bool edge_can_be_collapsed(size_t edge_idx) const;
        bool collapse_would_cause_fold_over(size_t edge_idx,
                const VectorF& v) const;
        bool faces_would_flip(size_t i1, size_t i2, const VectorF& v,
                const std::vector<size_t>& faces) const;
        bool face_would_flip(const VectorF& v_old, const VectorF& v_new,
                const VectorF& v_o1, const VectorF& v_o2) const;
        void collapse_edge(size_t edge_idx);
        VectorF get_vertex(size_t i) const;
        Float min_edge_length() const;
        Float compute_edge_length(const Edge& e) const;
        size_t get_num_vertices() const;
        size_t get_num_faces() const;
        size_t get_dim() const;

    private:
        std::vector<size_t> m_vertex_map;
        std::vector<Edge> m_edges;
        IndexHeap<Float> m_heap;
        std::vector<std::vector<size_t> > m_vertex_face_neighbors;

        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_face_indices;
        VectorI  m_importance;

        std::vector<VectorF> m_new_vertices;

        size_t m_num_collapsed;

    private:
        static const size_t UNMAPPED;
        static const Float INFINITE;
};

}
