/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <tetgen.h>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class Mesh;

class TetgenWrapper {
    public:
        TetgenWrapper(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        void run(const std::string& opt);

        MatrixFr get_vertices() { return m_tet_vertices; }
        MatrixIr get_faces() { return m_tet_faces; }
        MatrixIr get_voxels() { return m_tet_voxels; }

    private:
        MatrixFr m_ori_vertices;
        MatrixIr m_ori_faces;

        MatrixFr m_tet_vertices;
        MatrixIr m_tet_faces;
        MatrixIr m_tet_voxels;
};

}
