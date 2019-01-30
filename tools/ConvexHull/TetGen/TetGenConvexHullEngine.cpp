/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#ifdef WITH_TETGEN
#include "TetGenConvexHullEngine.h"

#include <cassert>
#include <Core/Exception.h>
#include <TetGen/TetgenWrapper.h>
#include <MeshUtils/Boundary.h>

using namespace PyMesh;

void TetGenConvexHullEngine::run(const MatrixFr& points) {
    TetgenWrapper tetgen;
    tetgen.set_keep_convex_hull(true);
    tetgen.set_verbosity(0);
    tetgen.set_points(points);
    tetgen.run();

    m_vertices = tetgen.get_vertices();
    m_faces = tetgen.get_faces();

    assert(points.rows() == m_vertices.rows());

    m_index_map.setLinSpaced(points.rows(), 0, points.rows()-1);
}

#endif
