/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>

namespace PyMesh {

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

        /**
         * Assign an integer importance per vertex.  Vertex with high importance
         * level would keep their coordinate during vertex merging.
         * Vertices with negative importance is excluded from consideration.
         */
        void set_importance_level(const VectorI& level) { m_importance_level = level; }

        /**
         * index map maps the input vertex index to an output vertex index.
         * i.e. it specifies where each input vertex ends up in the output.
         */
        VectorI get_index_map() const { return m_index_map; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_index_map;
        VectorI  m_importance_level;
};

}
