/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CGALConvexHull2D.h"

#include <sstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>

#include <Core/Exception.h>

using namespace PyMesh;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef std::vector<Point_2> Points;

void CGALConvexHull2D::run(const MatrixFr& points) {
    const size_t num_pts = points.rows();
    const size_t dim = points.cols();
    if (dim != 2) {
        std::stringstream err_msg;
        err_msg << "Invalid dim: " << dim << "  Expect dim=2.";
        throw RuntimeError(err_msg.str());
    }

    Points pts, result;
    for (size_t i=0; i<num_pts; i++) {
        pts.push_back(Point_2(points(i,0), points(i,1)));
    }

    CGAL::convex_hull_2(pts.begin(), pts.end(), std::back_inserter(result));

    const size_t num_vertices = result.size();
    m_vertices.resize(num_vertices, dim);
    m_faces.resize(num_vertices, 2);
    for (size_t i=0; i<num_vertices; i++) {
        const Point_2& p = result[i];
        m_vertices.row(i) = Vector2F(p.x(), p.y());
        m_faces.row(i) = Vector2I(i, (i+1)%num_vertices);
    }

    compute_index_map(points);
    reorient_faces();
}
