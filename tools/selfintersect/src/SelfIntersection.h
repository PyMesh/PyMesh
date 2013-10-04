#pragma once

#include "Eigen/Dense"
#include "CGAL_includes.hpp"
#include <vector>

class SelfIntersection {
    public:
        SelfIntersection(
                const Eigen::MatrixXd& V,
                const Eigen::MatrixXi& F);

        void set_mesh(Triangles& T) { m_mesh = T; }

    public:
        bool detect();
        bool resolve();
        bool validate();

        Eigen::MatrixXi get_offending_triangles();

    public:
        void validate_box_intersection(const Box& box_1, const Box& box_2);
        void resolve_box_intersection(const Box& box_1, const Box& box_2);

    private:
        std::vector<Box> get_triangle_bboxes();
        void initialize_resolved_mesh();
        void copy_unoffending_triangles();
        void triangulate_offending_triangles();

    private:
        class Face {
            public:
                Face() {
                    idx[0] = idx[1] = idx[2] = 0;
                }
                Face(size_t f1, size_t f2, size_t f3) {
                    idx[0] = f1;
                    idx[1] = f2;
                    idx[2] = f3;
                }
                size_t& operator[](size_t i) {
                    return idx[i];
                }
            private:
                size_t idx[3];
        };
        Triangles m_mesh;
        std::vector<Point_3> m_vertices;
        std::vector<Face> m_faces;

        Triangles m_resolved_mesh;
        std::vector<Point_3> m_resolved_vertices;
        std::vector<Face> m_resolved_faces;

        typedef std::pair<size_t, size_t> IndexPair;
        std::vector<IndexPair> m_intersecting_triangle_pairs;

        typedef std::map<IndexPair, size_t> EdgeMap;
        EdgeMap m_edge_map;

        typedef std::list<CGAL::Object> IntersectionObjects;
        std::vector<IntersectionObjects> m_intersection_objects;
};
