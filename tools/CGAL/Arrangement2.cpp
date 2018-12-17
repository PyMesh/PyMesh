/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#include <cassert>
#include "Arrangement2.h"

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/HashGrid.h>

#include <CGAL/Arr_batched_point_location.h>
#include <CGAL/Arr_point_location_result.h>

#ifdef WITH_CGAL

using namespace PyMesh;

void Arrangement2::run() {
    const size_t num_pts = m_points.rows();

    std::vector<Point_2> points;
    points.reserve(num_pts);
    for (size_t i=0; i<num_pts; i++) {
        points.emplace_back(m_points(i, 0), m_points(i, 1));
    }

    const size_t num_segments = m_segments.rows();
    std::vector<Segment_2> segments;
    segments.reserve(num_segments);
    for (size_t i=0; i<num_segments; i++) {
        segments.emplace_back(
                points[m_segments(i, 0)],
                points[m_segments(i, 1)]);
    }

    CGAL::insert(m_arrangement, segments.begin(), segments.end());
    CGAL::is_valid(m_arrangement);

    const size_t num_vertices = m_arrangement.number_of_vertices();
    m_vertices.resize(num_vertices, 2);
    size_t count = 0;
    for (auto itr=m_arrangement.vertices_begin();
            itr != m_arrangement.vertices_end(); itr++) {
        m_vertices(count, 0) = CGAL::to_double(itr->point()[0]);
        m_vertices(count, 1) = CGAL::to_double(itr->point()[1]);
        itr->set_data(count);
        count++;
    }

    const size_t num_edges = m_arrangement.number_of_edges();
    m_edges.resize(num_edges, 2);
    count = 0;
    for (auto itr=m_arrangement.edges_begin();
            itr != m_arrangement.edges_end(); itr++) {
        m_edges(count, 0) = itr->source()->data();
        m_edges(count, 1) = itr->target()->data();
        itr->set_data(count);
        itr->twin()->set_data(count);
        count++;
    }

    count=0;
    for (auto itr=m_arrangement.faces_begin();
            itr != m_arrangement.faces_end(); itr++) {
        itr->set_data(count);
        count++;
    }
}

std::vector<std::pair<Arrangement2::ElementType, int>> Arrangement2::query(
        const Matrix2Fr& pts) {
    using Point_location_result = CGAL::Arr_point_location_result<Arrangement_2>;
    using Query_result = std::pair<Point_2, Point_location_result::Type>;

    const size_t num_pts = pts.rows();
    auto less_than = [&pts](size_t i, size_t j) {
        if (pts(i,0) < pts(j,0)) return true;
        else if (pts(i,0) > pts(j,0)) return false;
        else if (pts(i,1) < pts(j,1)) return true;
        else if (pts(i,1) > pts(j,1)) return false;
        else return false;
    };
    std::vector<size_t> indices(num_pts);
    for (size_t i=0; i<num_pts; i++) {
        indices[i] = i;
    }
    std::sort(indices.begin(), indices.end(), less_than);

    std::vector<Point_2> query_pts;
    query_pts.reserve(num_pts);
    for (size_t i=0; i<num_pts; i++) {
        query_pts.emplace_back(pts(i,0), pts(i,1));
    }
    std::vector<Query_result> query_results;

    CGAL::locate(m_arrangement, query_pts.begin(), query_pts.end(),
            std::back_inserter(query_results));
    assert(query_results.size() == num_pts);

    std::vector<std::pair<ElementType, int>> results(num_pts);

    for (size_t i=0; i<num_pts; i++) {
        const auto& r = query_results[i];

        const auto v = boost::get<Arrangement_2::Vertex_const_handle>(&(r.second));
        const auto e = boost::get<Arrangement_2::Halfedge_const_handle>(&(r.second));
        const auto f = boost::get<Arrangement_2::Face_const_handle>(&(r.second));

        size_t index = indices[i];
        if (f) {
            results[index] = {CELL, (*f)->data()};
        } else if (e) {
            results[index] = {SEGMENT, (*e)->data()};
        } else if (v) {
            results[index] = {POINT, (*v)->data()};
        } else {
            throw NotImplementedError("Unknown CGAL query result type.");
        }
    }


    return results;
}

#endif
