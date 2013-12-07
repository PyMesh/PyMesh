#pragma once
#include <vector>

#include <Core/EigenTypedef.h>

#include "EdgeMap.h"

class ShortEdgeRemoval {
    public:
        ShortEdgeRemoval(MatrixF& vertices, MatrixI& faces);

    public:
        size_t run(Float threshold);
        MatrixF get_vertices();
        MatrixI get_faces();

    private:
        void init_vertex_map();
        void init_edge_face_adjacency();
        void compute_edge_lengths();

        size_t collapse(Float threshold);
        bool can_be_collapsed(size_t ext_idx) const;
        void collapse_edge(size_t ext_idx);
        Float compute_edge_length(const Edge& e) const;

    private:
        std::vector<Float> m_edge_lengths;
        std::vector<size_t> m_vertex_map;
        std::vector<Edge> m_edges;

        MatrixF m_vertices;
        MatrixI m_faces;

        std::vector<Vector3F> m_new_vertices;
};
