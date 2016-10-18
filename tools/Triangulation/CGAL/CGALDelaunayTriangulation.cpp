#include "CGALDelaunayTriangulation.h"

#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

using namespace PyMesh;

void CGALDelaunayTriangulation::run() {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Triangulation_vertex_base_with_info_2<size_t, Kernel> Vb;
    typedef CGAL::Triangulation_data_structure_2<Vb> Tds;
    typedef CGAL::Delaunay_triangulation_2<Kernel, Tds> Delaunay;
    typedef Kernel::Point_2 Point;

    const size_t num_pts = m_vertices.rows();
    std::vector<std::pair<Point, size_t> > pts;
    for (size_t i=0; i<num_pts; i++) {
        pts.push_back({Point(m_vertices(i, 0), m_vertices(i, 1)), i});
    }

    Delaunay t;
    t.insert(pts.begin(), pts.end());

    m_faces.resize(t.number_of_faces(), 3);
    size_t count = 0;
    for (auto itr = t.finite_faces_begin();
            itr != t.finite_faces_end(); itr++) {
        m_faces(count, 0)  = itr->vertex(0)->info();
        m_faces(count, 1)  = itr->vertex(1)->info();
        m_faces(count, 2)  = itr->vertex(2)->info();
        count++;
    }
}
