/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include "LexicographicTriangulation.h"

#include <igl/copyleft/cgal/lexicographic_triangulation.h>

using namespace PyMesh;

void LexicographicTriangulation::run() {
    igl::copyleft::cgal::lexicographic_triangulation(m_vertices, m_faces);
}
