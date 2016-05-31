/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>

namespace PyMesh {

class IsolatedVertexRemoval {
    public:
        IsolatedVertexRemoval(const MatrixFr& vertices, const MatrixIr& faces);
    public:
        size_t run();
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI get_ori_vertex_indices() const { return m_ori_vertex_indices; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI m_ori_vertex_indices;
};

}
