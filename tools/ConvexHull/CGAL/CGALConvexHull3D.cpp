/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CGALConvexHull3D.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

#include <cassert>
#include <sstream>
#include <vector>

#include <Core/Exception.h>

using namespace PyMesh;

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_items_with_id_3          Items;
typedef CGAL::Polyhedron_3<K, Items>              Polyhedron_3;
typedef K::Point_3                                Point_3;

void CGALConvexHull3D::run(const MatrixFr& points) {
    std::list<Point_3> cgal_pts;
    const size_t num_pts = points.rows();
    const size_t dim = points.cols();
    if (dim != 3) {
        std::stringstream err_msg;
        err_msg << "Invalid dim: " << dim << "  Expect dim=3.";
        throw RuntimeError(err_msg.str());
    }

    for (size_t i=0; i<num_pts; i++) {
        const VectorF& p = points.row(i);
        cgal_pts.push_back(Point_3(p[0], p[1], p[2]));
    }

    Polyhedron_3 hull;
    CGAL::convex_hull_3(cgal_pts.begin(), cgal_pts.end(), hull);
    assert(hull.is_closed());
    assert(hull.is_pure_triangle());

    const size_t num_vertices = hull.size_of_vertices();
    const size_t num_faces = hull.size_of_facets();

    m_vertices.resize(num_vertices, dim);
    m_faces.resize(num_faces, 3);

    size_t vertex_count=0;
    for (auto itr=hull.vertices_begin(); itr!=hull.vertices_end(); itr++) {
        const Point_3& p = itr->point();
        m_vertices.coeffRef(vertex_count, 0) = p.x();
        m_vertices.coeffRef(vertex_count, 1) = p.y();
        m_vertices.coeffRef(vertex_count, 2) = p.z();
        itr->id() = vertex_count;
        vertex_count++;
    }

    size_t face_count=0;
    for (auto f_itr=hull.facets_begin(); f_itr!=hull.facets_end(); f_itr++) {
        size_t edge_count=0;
        auto h_itr = f_itr->facet_begin();
        do {
            m_faces.coeffRef(face_count, edge_count) = h_itr->vertex()->id();
            edge_count++;
            h_itr++;
        } while (h_itr != f_itr->facet_begin());
        face_count++;
    }

    compute_index_map(points);
    reorient_faces();
}

