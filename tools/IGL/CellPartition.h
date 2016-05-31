/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#include <memory>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

namespace PyMesh {

class CellPartition {
    public:
        typedef std::shared_ptr<CellPartition> Ptr;
        static Ptr create(const Mesh::Ptr& mesh);
        static Ptr create_raw(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        CellPartition(const MatrixFr& vertices, const MatrixIr& faces) :
            m_vertices(vertices), m_faces(faces) { }

        void run();

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_source_faces() const { return m_source_faces; }

        size_t get_num_cells() const;
        MatrixIr get_cell_faces(const size_t cell_id) const;
        MatrixIr get_cells() const { return m_cells; }

        size_t get_num_patches() const;
        VectorI get_patches() const { return m_patches; }

        MatrixIr get_winding_number() const { return m_winding_number; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_source_faces;
        MatrixIr m_cells;
        VectorI  m_patches;
        MatrixIr m_winding_number;
};

}
