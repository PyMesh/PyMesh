/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#include <memory>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

namespace PyMesh {

class MinkowskiSum {
    public:
        typedef std::shared_ptr<MinkowskiSum> Ptr;
        static Ptr create(const Mesh::Ptr& mesh);
        static Ptr create_raw(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        MinkowskiSum(const MatrixFr& vertices, const MatrixIr& faces):
            m_vertices(vertices), m_faces(faces) { }

        void run(const MatrixFr& path);

        MatrixFr get_vertices() const { return m_out_vertices; }
        MatrixIr get_faces() const { return m_out_faces; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixFr m_out_vertices;
        MatrixIr m_out_faces;
};

}
