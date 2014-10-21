#pragma once
#include <Core/EigenTypedef.h>

/**
 * This class removes duplicated vertices and nothing else.  In particular,
 * nothing is done to faces that become degenerated after the removal.
 */
class DuplicatedVertexRemoval {
    public:
        DuplicatedVertexRemoval(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        size_t run(Float tol);
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI get_index_map() const { return m_index_map; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_index_map;
};
