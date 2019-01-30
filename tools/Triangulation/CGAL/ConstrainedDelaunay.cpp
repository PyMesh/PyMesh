/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_CGAL
#include "ConstrainedDelaunay.h"

#include <vector>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>

using namespace PyMesh::_CGAL;

void ConstrainedDelaunay::run() {
    using K = ::CGAL::Exact_predicates_exact_constructions_kernel;
    using Vb = ::CGAL::Triangulation_vertex_base_with_info_2<size_t, K>;
    using Fb = ::CGAL::Constrained_triangulation_face_base_2<K>; 
    using TDS = ::CGAL::Triangulation_data_structure_2<Vb, Fb>;
    using Tag = ::CGAL::Exact_intersections_tag;
    using CDT = ::CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Tag>;
    using CDTP = ::CGAL::Constrained_triangulation_plus_2<CDT>;
    using Point = CDT::Point;

    CDTP t;

    const size_t num_pts = m_points.rows();
    std::vector<CDT::Vertex_handle> handles;
    handles.reserve(num_pts);
    for (size_t i=0; i<num_pts; i++) {
        auto handle = t.insert(Point(m_points(i,0), m_points(i, 1)));
        handles.push_back(handle);
    }
    assert(t.is_valid());

    const size_t num_segments = m_segments.rows();
    for (size_t i=0; i<num_segments; i++) {
        if (m_segments(i,0) == m_segments(i,1)) continue;
        t.insert_constraint(
                handles[m_segments(i,0)],
                handles[m_segments(i,1)]);
    }
    assert(t.is_valid());

    m_vertices.resize(t.number_of_vertices(), 2);
    size_t count = 0;
    for (auto itr=t.finite_vertices_begin();
            itr != t.finite_vertices_end(); itr++) {
        itr->info() = count;
        m_vertices(count, 0) = ::CGAL::to_double(itr->point()[0]);
        m_vertices(count, 1) = ::CGAL::to_double(itr->point()[1]);
        count++;
    }

    m_faces.resize(t.number_of_faces(), 3);
    count = 0;
    for (auto itr = t.finite_faces_begin();
            itr != t.finite_faces_end(); itr++) {
        m_faces(count, 0)  = itr->vertex(0)->info();
        m_faces(count, 1)  = itr->vertex(1)->info();
        m_faces(count, 2)  = itr->vertex(2)->info();
        count++;
    }
}

#endif
