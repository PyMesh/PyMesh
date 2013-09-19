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

    private:
        std::vector<Box> get_triangle_bboxes();

    private:
        Triangles m_mesh;
        typedef std::pair<size_t, size_t> IndexPair;
        std::vector<IndexPair> m_intersection;
};
