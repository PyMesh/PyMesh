/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <functional>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Misc/TripletMap.h>

namespace PyMesh {

/**
 * Split edges that match a customized criterion
 */
class EdgeSplitter {
    public:
        EdgeSplitter(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        typedef std::function<bool(const VectorF& v1, const VectorF& v2)> IndicatorFunc;
        void run(IndicatorFunc split_indicator, Float max_length);

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }

    private:
        void clear_intermediate_data();
        void generate_edge_map();
        void set_all_faces_as_valid();
        void split_edge(const VectorI& edge,
                const std::vector<size_t>& adj_faces, Float max_length);
        void finalize_geometry();

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;

        TripletMap<size_t> m_edge_adj_faces;
        std::vector<bool> m_face_is_valid;
        std::vector<VectorF> m_new_vertices;
        std::vector<Vector3I> m_new_faces;
};

}
