/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <vector>

#include <Core/EigenTypedef.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>

namespace PyMesh {

class Arrangement2 {
    public:
        Arrangement2() = default;

        enum ElementType {
            POINT,
            SEGMENT,
            CELL
        };

    public:
        void set_points(const Matrix2Fr& points) { m_points = points; }
        const Matrix2Fr& get_points() const { return m_points; }

        void set_segments(const Matrix2Ir& segments) { m_segments = segments; }
        const Matrix2Ir& get_segments() const { return m_segments; }

        void run();

        const Matrix2Fr get_vertices() const { return m_vertices; }
        const Matrix2Ir get_edges() const { return m_edges; }
        size_t get_num_faces() const { return m_arrangement.number_of_faces(); }

        std::vector<std::pair<ElementType, int>> query(const Matrix2Fr& pts);

    private:
        using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
        using Traits = CGAL::Arr_segment_traits_2<Kernel>;
        using Point_2 = Traits::Point_2;
        using Segment_2 = Traits::X_monotone_curve_2;
        using Dcel = CGAL::Arr_extended_dcel<Traits, int, int, int>;
        using Arrangement_2 = CGAL::Arrangement_2<Traits, Dcel>;

        Matrix2Fr m_points;
        Matrix2Ir m_segments;
        Arrangement_2 m_arrangement;
        Matrix2Fr m_vertices;
        Matrix2Ir m_edges;
};

}

#endif
