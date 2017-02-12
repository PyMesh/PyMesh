#include "GeoGramEngine.h"
#include <Core/Exception.h>

#include <geogram/basic/common.h>
#include <geogram/delaunay/delaunay.h>
#include <geogram/mesh/mesh.h>

#include <iostream>

using namespace PyMesh;

void GeoGramEngine::run() {
    GEO::Delaunay_var delaunay = GEO::Delaunay::create(3, "tetgen");
    delaunay->initialize();
    GEO::Mesh mesh;

    // Import mesh
    if (delaunay->supports_constraints()) {
        const size_t num_vertices = m_vertices.rows();
        for (size_t i=0; i<num_vertices; i++) {
            mesh.vertices.create_vertex(m_vertices.row(i).data());
        }
        const size_t num_faces = m_faces.rows();
        for (size_t i=0; i<num_faces; i++) {
            mesh.facets.create_triangle(m_faces(i,0), m_faces(i,1), m_faces(i,2));
        }
        mesh.show_stats();
        mesh.assert_is_valid();
        delaunay->set_constraints(&mesh);
    } else {
        throw NotImplementedError("");
        std::cerr << "GeoGram Delaunay does not support constraints." <<
            std::endl;
        delaunay->set_vertices(m_vertices.rows(), m_vertices.data());
    }

    // Run!
    try {
        delaunay->set_vertices(0,nil); // No additional vertex 
    } catch (const GEO::Delaunay::InvalidInput& error_report) {
        size_t num_bad_faces = error_report.invalid_facets.size();
        std::stringstream err_msg;
        err_msg << "GeoGram delaunay falied: Encountered "
            << num_bad_faces << " bad faces.";
        throw RuntimeError(err_msg.str());
    }

    // Extract results
    const size_t num_vertices = delaunay->nb_vertices();
    m_vertices.resize(num_vertices, 3);
    for (size_t i=0; i<num_vertices; i++) {
        auto coord = delaunay->vertex_ptr(i);
        m_vertices(i,0) = coord[0];
        m_vertices(i,1) = coord[1];
        m_vertices(i,2) = coord[2];
    }

    const size_t num_tets = delaunay->nb_cells();
    m_voxels.resize(num_tets, 4);
    for (size_t i=0; i<num_tets; i++) {
        m_voxels(i,0) = delaunay->cell_vertex(i,0);
        m_voxels(i,1) = delaunay->cell_vertex(i,1);
        m_voxels(i,2) = delaunay->cell_vertex(i,2);
        m_voxels(i,3) = delaunay->cell_vertex(i,3);
    }
}
