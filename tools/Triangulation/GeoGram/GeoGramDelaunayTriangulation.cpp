#include "GeoGramDelaunayTriangulation.h"
#include <geogram/delaunay/delaunay.h>

using namespace PyMesh;

void GeoGramDelaunayTriangulation::run() {
    GEO::Delaunay_var delaunay = GEO::Delaunay::create(2,"triangle");
    delaunay->set_vertices(m_vertices.rows(), m_vertices.data());
    const size_t num_faces = delaunay->nb_cells();
    const GEO::signed_index_t* faces = delaunay->cell_to_v();

    m_faces.resize(num_faces, 3);
    for (size_t i=0; i<num_faces; i++) {
        m_faces(i, 0) = (faces + i*3)[0];
        m_faces(i, 1) = (faces + i*3)[1];
        m_faces(i, 2) = (faces + i*3)[2];
    }
}
