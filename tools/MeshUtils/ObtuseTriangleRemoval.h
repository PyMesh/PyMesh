#pragma once
#include <vector>
#include <EigenTypedef.h>
#include <EdgeMap.h>

class ObtuseTriangleRemoval {
    public:
        ObtuseTriangleRemoval(MatrixF& vertices, MatrixI& faces);

    public:
        size_t run(Float max_angle_allowed);
        MatrixF get_vertices();
        MatrixI get_faces();

    private:
        void set_all_faces_as_valid();
        void compute_face_angles();
        void compute_opposite_vertices();
        void compute_edge_face_adjacency();
        bool edge_can_be_splited(size_t ext_idx) const;

        size_t split_obtuse_triangles(Float max_angle);
        void split_triangle(size_t ext_idx);
        Vector3F project(size_t ext_idx);

    private:
        typedef EdgeMap<size_t> EdgeMapI;
        typedef EdgeMapI::ValueType AdjFaces;

        std::vector<Float> m_face_angles;
        std::vector<size_t> m_opp_vertices;
        std::vector<Edge> m_edges;
        std::vector<bool> m_valid;
        EdgeMapI m_edge_faces;
        std::vector<Vector3F> m_new_vertices;
        std::vector<Vector3I> m_new_faces;

        MatrixF m_vertices;
        MatrixI m_faces;
};
