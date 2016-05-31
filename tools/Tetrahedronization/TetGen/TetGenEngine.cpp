/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TetGenEngine.h"

#include <cmath>
#include <sstream>
#include <tetgen/TetgenWrapper.h>

using namespace PyMesh;

void TetGenEngine::run() {
    preprocess();

    // Use the volume of regular tetrahedron of radius m_cell_size as max
    // volume.
    Float max_tet_volume = 8 * pow(m_cell_size, 3) / (9 * sqrt(3));

    TetgenWrapper tetgen(m_vertices, m_faces);
    std::stringstream opt;
    opt << "pqQ";
    opt << "a" << max_tet_volume;
    tetgen.run(opt.str());

    m_vertices = tetgen.get_vertices();
    m_faces = tetgen.get_faces();
    m_voxels = tetgen.get_voxels();
}
