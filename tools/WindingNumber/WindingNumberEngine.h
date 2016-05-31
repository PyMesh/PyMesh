/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class WindingNumberEngine {
    public:
        typedef std::shared_ptr<WindingNumberEngine> Ptr;
        static Ptr create(const std::string& engine_name);

    public:
        virtual ~WindingNumberEngine() {}

    public:
        virtual VectorF run(const MatrixFr& queries)=0;

        void set_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
            m_vertices = vertices;
            m_faces = faces;
        }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
};

}
