#pragma once
#include <Core/EigenTypedef.h>

class IsolatedVertexRemoval {
    public:
        IsolatedVertexRemoval(const MatrixFr& vertices, const MatrixIr& faces);
    public:
        size_t run();
        const MatrixFr& get_vertices() const { return m_vertices; }
        const MatrixIr& get_faces() const { return m_faces; }
        const VectorI& get_ori_vertex_indices() const { return m_ori_vertex_indices; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI m_ori_vertex_indices;
};
