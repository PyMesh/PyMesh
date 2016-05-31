/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <vector>
#include <Core/EigenTypedef.h>
#include <Misc/Triplet.h>
#include <Misc/TripletMap.h>

namespace PyMesh {
class ObtuseTriangleRemoval {
    public:
        ObtuseTriangleRemoval(MatrixFr& vertices, MatrixIr& faces);

    public:
        // Angle in radian
        size_t run(Float max_angle_allowed, size_t max_iterations=1);
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }

    private:
        void clear_intermediate_data();
        void set_all_faces_as_valid();
        void compute_face_angles();
        void compute_opposite_vertices();
        void compute_edge_face_adjacency();
        bool edge_can_be_splited(size_t ext_idx) const;

        size_t split_obtuse_triangles(Float max_angle);
        void split_triangle(size_t ext_idx);
        Vector3F project(size_t ext_idx);

        void finalize_geometry();
        void finalize_vertices();
        void finalize_faces();

    private:
        typedef Triplet Edge;
        typedef TripletMap<size_t> EdgeMapI;
        typedef EdgeMapI::ValueType AdjFaces;

        std::vector<Float> m_face_angles;
        std::vector<size_t> m_opp_vertices;
        std::vector<Edge> m_edges;
        std::vector<bool> m_valid;
        EdgeMapI m_edge_faces;
        std::vector<Vector3F> m_new_vertices;
        std::vector<Vector3I> m_new_faces;

        MatrixFr m_vertices;
        MatrixIr m_faces;
};
}
