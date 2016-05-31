/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TetgenWrapper.h"
#include <iostream>
#include <tetgen.h>

#include <Core/Exception.h>
#include <Mesh.h>

#include "TetgenMeshConverter.h"
#include "TetgenException.h"

using namespace PyMesh;

TetgenWrapper::TetgenWrapper(const MatrixFr& vertices, const MatrixIr& faces) :
    m_ori_vertices(vertices),
    m_ori_faces(faces) {
        size_t dim = m_ori_vertices.cols();
        size_t num_vertex_per_face = m_ori_faces.cols();
        if (dim != 3 || num_vertex_per_face != 3) {
            throw NotImplementedError("Only 3D triangular mesh is supported");
        }
}


void TetgenWrapper::run(const std::string& opt) {
    MatrixIr tets(0, 4);
    TetgenMeshConverter in_converter(m_ori_vertices, m_ori_faces, tets);

    tetgenio in, out;
    in_converter.get_tetgen_mesh(in);

    try {
        tetrahedralize(const_cast<char*>(opt.c_str()), &in, &out);
    } catch (int error_code) {
        throw TetgenException(error_code);
    }

    TetgenMeshConverter out_converter(out);
    m_tet_vertices = out_converter.get_vertices();
    m_tet_faces = out_converter.get_faces();
    m_tet_voxels = out_converter.get_tets();
}
