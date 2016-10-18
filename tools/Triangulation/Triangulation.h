/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <memory>
#include <Core/EigenTypedef.h>

namespace PyMesh {
    class Triangulation {
        public:
            typedef std::shared_ptr<Triangulation> Ptr;
            static Ptr create(const std::string& engine_name);

        public:
            Triangulation() = default;
            virtual ~Triangulation() = default;

        public:
            void set_vertices(const MatrixFr& vertices) {
                m_vertices = vertices;
            }

            virtual void run() = 0;

            const MatrixIr& get_faces() const {
                return m_faces;
            }

        protected:
            MatrixFr m_vertices;
            MatrixIr m_faces;
    };
}
