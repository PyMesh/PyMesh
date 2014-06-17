#pragma
#include <vector>
#include <Core/EigenTypedef.h>

/**
 * This class removes duplicated vertices and nothing else.  In particular,
 * nothing is done to faces that become degenerated after the removal.
 */
class DuplicatedVertexRemoval {
    public:
        DuplicatedVertexRemoval(MatrixFr& vertices, MatrixIr& faces);

    public:
        size_t run(Float tol);
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
};
