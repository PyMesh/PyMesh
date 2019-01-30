/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#ifdef WITH_TETGEN
#include "TetGenEngine.h"

#include <cmath>
#include <sstream>
#include <TetGen/TetgenWrapper.h>

using namespace PyMesh;

void TetGenEngine::run() {
    preprocess();

    // Use the volume of regular tetrahedron of radius m_cell_size as max
    // volume.
    Float max_tet_volume = 8 * pow(m_cell_size, 3) / (9 * sqrt(3));

    TetgenWrapper tetgen;
    tetgen.set_points(m_vertices);
    tetgen.set_triangles(m_faces);
    tetgen.set_verbosity(1);
    tetgen.set_max_tet_volume(max_tet_volume);
    tetgen.set_max_radius_edge_ratio(m_cell_radius_edge_ratio);
    tetgen.run();

    m_vertices = tetgen.get_vertices();
    m_faces = tetgen.get_faces();
    m_voxels = tetgen.get_voxels();
}

#endif
