/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>
#include <tetgen.h>

namespace PyMesh {

class TetgenMeshConverter {
    public:
        TetgenMeshConverter(
                const MatrixFr& vertices,
                const MatrixIr& faces,
                const MatrixIr& tets) :
            m_vertices(vertices), m_faces(faces), m_tets(tets) {}
        TetgenMeshConverter(const tetgenio& mesh);

    public:
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        MatrixIr get_tets() const { return m_tets; }
        void get_tetgen_mesh(tetgenio& mesh) const;

    private:
        void extract_vertices(const tetgenio& mesh);
        void extract_faces(const tetgenio& mesh);
        void extract_tets(const tetgenio& mesh);

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_tets;
};

}
