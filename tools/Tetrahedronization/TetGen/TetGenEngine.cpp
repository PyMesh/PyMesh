#include "TetGenEngine.h"

#include <sstream>
#include <tetgen/TetgenWrapper.h>

void TetGenEngine::run() {
    TetgenWrapper tetgen(m_vertices, m_faces);
    std::stringstream opt;
    opt << "pqQ";
    opt << "a" << m_cell_size;
    tetgen.run(opt.str());

    m_vertices = tetgen.get_vertices();
    m_faces = tetgen.get_faces();
    m_voxels = tetgen.get_voxels();
}
