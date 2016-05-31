/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TetgenMeshConverter.h"

using namespace PyMesh;

TetgenMeshConverter::TetgenMeshConverter(const tetgenio& mesh) {
    extract_vertices(mesh);
    extract_faces(mesh);
    extract_tets(mesh);
}

void TetgenMeshConverter::extract_vertices(const tetgenio& mesh) {
    const size_t num_vertices = mesh.numberofpoints;
    const size_t num_entries = num_vertices * 3;
    m_vertices.resize(num_vertices, 3);
    std::copy(mesh.pointlist, mesh.pointlist + num_entries,
            m_vertices.data());
}

void TetgenMeshConverter::extract_faces(const tetgenio& mesh) {
    const size_t num_faces = mesh.numberoftrifaces;
    const size_t num_entries = num_faces * 3;
    m_faces.resize(num_faces, 3);
    std::copy(mesh.trifacelist, mesh.trifacelist + num_entries,
            m_faces.data());
}

void TetgenMeshConverter::extract_tets(const tetgenio& mesh) {
    const size_t num_tets = mesh.numberoftetrahedra;
    const size_t num_entries = num_tets * 4;
    m_tets.resize(num_tets, 4);
    std::copy(mesh.tetrahedronlist, mesh.tetrahedronlist + num_entries,
            m_tets.data());
}

void TetgenMeshConverter::get_tetgen_mesh(tetgenio& mesh) const {
    mesh.deinitialize();
    mesh.initialize();

    size_t num_vertex_per_face = 3;
    mesh.firstnumber=0;
    mesh.mesh_dim=3;

    size_t num_vertices = m_vertices.rows();
    mesh.numberofpoints = num_vertices;
    mesh.pointlist = new double[num_vertices * 3];
    std::copy(m_vertices.data(), m_vertices.data() + num_vertices * 3, mesh.pointlist);

    size_t num_faces = m_faces.rows();
    mesh.numberoffacets = num_faces;
    mesh.facetlist = new tetgenio::facet[mesh.numberoffacets];
    mesh.facetmarkerlist = new int[num_faces];

    for (size_t i=0; i<num_faces; i++) {
        tetgenio::facet* f = &mesh.facetlist[i];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
        f->numberofholes = 0;
        f->holelist = NULL;

        tetgenio::polygon* p = &f->polygonlist[0];
        p->numberofvertices=num_vertex_per_face;
        p->vertexlist = new int[p->numberofvertices];
        std::copy(m_faces.data() + i*num_vertex_per_face,
                m_faces.data() + i*num_vertex_per_face + num_vertex_per_face,
                p->vertexlist);
        mesh.facetmarkerlist[i] = 0; // Not sure what it does.
    }
}
