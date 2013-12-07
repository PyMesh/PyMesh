#include "tetgen_wrapper.h"
#include <iostream>
#include <tetgen.h>

#include <Core/Exception.h>
#include <Mesh.h>

//TetgenWrapper::TetgenWrapper(Mesh& surface) :
//    m_ori_vertices(surface.get_vertices()),
//    m_ori_faces(surface.get_faces()) {
//        if (surface.get_vertex_per_face() != 3) {
//            throw NotImplementedError("Only triangular mesh is supported");
//        }
//}

TetgenWrapper::TetgenWrapper(const VectorF& vertices, const VectorI& faces,
        size_t num_vertex_per_face) :
    m_ori_vertices(vertices),
    m_ori_faces(faces) {
        if (num_vertex_per_face != 3) {
            throw NotImplementedError("Only triangular mesh is supported");
        }
}


int TetgenWrapper::tetgen(const std::string& opt) {
    tetgenio in = create_tetgen_mesh(m_ori_vertices, m_ori_faces);
    tetgenio out;

    try {
        tetrahedralize(const_cast<char*>(opt.c_str()), &in, &out);
    } catch (int error_code) {
        return error_code;
    }

    extract_vertices(out);
    extract_faces(out);
    extract_voxels(out);
    return 0;
}

void TetgenWrapper::extract_vertices(tetgenio& mesh) {
    size_t num_vertices = mesh.numberofpoints;
    size_t num_entries = num_vertices * 3;
    m_tet_vertices.resize(num_entries);
    //for (size_t i=0; i<num_entries; i++) {
    //    m_tet_vertices[i] = mesh.pointlist[i];
    //}
    std::copy(mesh.pointlist, mesh.pointlist + num_entries,
            m_tet_vertices.data());
}

void TetgenWrapper::extract_faces(tetgenio& mesh) {
    size_t num_faces = mesh.numberoftrifaces;
    size_t num_entries = num_faces * 3;
    m_tet_faces.resize(num_entries);
    //for (size_t i=0; i<num_entries; i++) {
    //    m_tet_faces[i] = mesh.trifacelist[i];
    //}
    std::copy(mesh.trifacelist, mesh.trifacelist + num_entries,
            m_tet_faces.data());
}

void TetgenWrapper::extract_voxels(tetgenio& mesh) {
    size_t num_voxels = mesh.numberoftetrahedra;
    size_t num_entries = num_voxels * 4;
    m_tet_voxels.resize(num_entries);
    //for (size_t i=0; i<num_entries; i++) {
    //    m_tet_voxels[i] = mesh.tetrahedronlist[i];
    //}
    std::copy(mesh.tetrahedronlist, mesh.tetrahedronlist + num_entries,
            m_tet_voxels.data());
}

tetgenio TetgenWrapper::create_tetgen_mesh(
        const VectorF& vertices, const VectorI& faces) {
    size_t num_vertex_per_face = 3;
    tetgenio mesh;
    mesh.firstnumber=0;
    mesh.mesh_dim=3;

    size_t num_vertices = vertices.size() / 3;
    mesh.numberofpoints = num_vertices;
    mesh.pointlist = new double[num_vertices * 3];
    std::copy(vertices.data(), vertices.data() + vertices.size(), mesh.pointlist);

    size_t num_faces = faces.size() / num_vertex_per_face;
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
        std::copy(faces.data() + i*num_vertex_per_face,
                faces.data() + i*num_vertex_per_face + num_vertex_per_face,
                p->vertexlist);
        mesh.facetmarkerlist[i] = 0; // Not sure what it does.
    }

    return mesh;
}

