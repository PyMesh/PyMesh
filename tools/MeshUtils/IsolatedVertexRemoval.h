#pragma once
#include <Core/EigenTypedef.h>

class IsolatedVertexRemoval {
    public:
        IsolatedVertexRemoval(const MatrixFr& vertices, const MatrixIr& faces);
    public:
        size_t run();
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
};
