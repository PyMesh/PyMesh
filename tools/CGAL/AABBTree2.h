/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <Core/EigenTypedef.h>

#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_segment_primitive.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <cassert>
#include <vector>
#include <memory>

namespace PyMesh {
class AABBTree2 {
    public:
        typedef std::shared_ptr<AABBTree> Ptr;

        typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
        typedef K::Point_3 Point;
        typedef K::Segment_3 Segment;

        typedef std::vector<Segment> Segments;
        typedef CGAL::AABB_segment_primitive<K, Segments::iterator> Primitive;
        typedef CGAL::AABB_traits<K, Primitive> AABB_segment_traits;
        typedef CGAL::AABB_tree<AABB_segment_traits> Tree;
        typedef std::shared_ptr<Tree> TreePtr;
        typedef Tree::Point_and_primitive_id Point_and_primitive_id;

    public:
        AABBTree2(const Matrix2Fr& points, const Matrix2Ir& segments) {
            const size_t num_segments = segments.rows();
            for (size_t i=0; i<num_segments; i++) {
                const Vector2I& s = segments.row(i);
                m_segments.emplace_back(
                        to_cgal_point(points.row(s[0])),
                        to_cgal_point(points.row(s[1])));
            }

            m_tree = std::make_shared<Tree>(m_segments.begin(), m_segments.end());
            m_tree->accelerate_distance_queries();
        }

    public:
        /**
         * Determine if each edge intersects the input segments.
         */
        VectorI do_intersect_segments(
                const Matrix2Fr& vertices, const Matrix2Ir& edges) {
            const size_t num_edges = edges.rows();
            VectorI results = VectorI::Zero(num_edges);
            for (size_t i=0; i<num_edges; i++) {
                Vector2I e = edges.row(i);
                const auto p1 = to_cgal_point(vertices.row(e[0]));
                const auto p2 = to_cgal_point(vertices.row(e[1]));
                Segment s(p1, p2);
                results[i] = m_tree->do_intersect(s);
            }
            return results;
        }

        void look_up(const Matrix2Fr& points,
                VectorF& squared_dists,
                VectorI& closest_segment_indices) {
            const size_t num_pts = points.rows();
            squared_dists.resize(num_pts);
            closest_segment_indices.resize(num_pts);

            for (size_t i=0; i<num_pts; i++) {
                Point p = to_cgal_point(points.row(i));
                Point_and_primitive_id itr = m_tree->closest_point_and_primitive(p);
                closest_segment_indices[i] = itr.second - m_segments.begin();
                Point p2 = itr.first;
                squared_dists[i] = (p-p2).squared_length();
            }
        }

        void look_up_with_closest_points(const Matrix2Fr& points,
                VectorF& squared_dists,
                VectorI& closest_segment_indices,
                Matrix2Fr& closest_points) {
            const size_t num_pts = points.rows();
            squared_dists.resize(num_pts);
            closest_segment_indices.resize(num_pts);
            closest_points.resize(num_pts, 2);

            for (size_t i=0; i<num_pts; i++) {
                Point p = to_cgal_point(points.row(i));
                Point_and_primitive_id itr = m_tree->closest_point_and_primitive(p);
                closest_segment_indices[i] = itr.second - m_segments.begin();
                Point p2 = itr.first;
                closest_points.row(i) = to_eigen_point(p2);
                squared_dists[i] = (p-p2).squared_length();
            }
        }

    private:
        Point to_cgal_point(const Vector2F& p) {
            return Point(p[0], p[1], 0.0);
        }

        VectorF to_eigen_point(const Point& p) {
            return Vector2F(p[0], p[1]);
        }

    private:
        TreePtr m_tree;
        Segments m_segments;
};

}

#endif
