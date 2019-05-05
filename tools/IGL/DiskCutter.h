/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL

#include <memory>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

namespace PyMesh {

class DiskCutter {
    public:
        using Ptr = std::shared_ptr<DiskCutter>;
        static Ptr create(const Mesh::Ptr mesh);
        static Ptr create_raw(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        DiskCutter(const MatrixFr& vertices, const MatrixIr& faces) :
            m_vertices(vertices), m_faces(faces) { }

        void run();

        const MatrixFr& get_vertices() const { return m_out_vertices; }
        const MatrixIr& get_faces() const { return m_out_faces; }

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixFr m_out_vertices;
        MatrixIr m_out_faces;
};

}

#endif
