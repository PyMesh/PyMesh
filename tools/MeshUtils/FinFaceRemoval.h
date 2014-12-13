#pragma once
#include <Core/EigenTypedef.h>

class FinFaceRemoval {
    public:
        FinFaceRemoval(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        size_t run();
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_face_indices() const { return m_face_indices; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_face_indices;
};
