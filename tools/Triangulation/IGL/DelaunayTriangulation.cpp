/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#ifdef WITH_IGL_AND_CGAL
#include "DelaunayTriangulation.h"

#include <igl/copyleft/cgal/delaunay_triangulation.h>

using namespace PyMesh;

void DelaunayTriangulation::run() {
    igl::copyleft::cgal::delaunay_triangulation(m_points, m_faces);
}

#endif
