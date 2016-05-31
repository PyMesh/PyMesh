/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>
#include <set>

#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

namespace PyMesh {

class SelfIntersection {
    public:
        typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
        typedef CGAL::Point_3<Kernel>    Point_3;
        typedef CGAL::Segment_3<Kernel>  Segment_3;
        typedef CGAL::Triangle_3<Kernel> Triangle_3;
        typedef std::vector<Point_3>     Points;

    public:
        SelfIntersection(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        /**
         * Detect triangle-triangle intersections for non-degenerated triangles.
         */
        void detect_self_intersection();

        void clear();

        MatrixIr get_self_intersecting_pairs() const {
            if (!m_intersecting_pairs.empty())
                return MatrixUtils::rowstack(m_intersecting_pairs);
            else
                return MatrixIr::Zero(0, 2);
        }

    public:
        void handle_intersection_candidate(size_t f_idx_1, size_t f_idx_2);

    private:
        std::vector<size_t> topological_overlap(size_t id1, size_t id2) const;

    private:
        std::vector<Vector2I> m_intersecting_pairs;
        Points m_points;
        MatrixIr m_faces;
};

}
