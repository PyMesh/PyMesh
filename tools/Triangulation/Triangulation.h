/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <memory>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {
    class Triangulation {
        public:
            typedef std::shared_ptr<Triangulation> Ptr;
            static Ptr create(const std::string& engine_name);

        public:
            Triangulation() = default;
            virtual ~Triangulation() = default;

        public:
            void set_points(const Matrix2Fr& points) {
                m_points = points;
            }

            void set_segments(const Matrix2Ir& segments) {
                m_segments = segments;
            }

            virtual void run() {
                throw NotImplementedError(
                        "Triangulation algorithm is not implemented");
            }

            const Matrix2Fr& get_vertices() const {
                return m_vertices;
            }

            const Matrix3Ir& get_faces() const {
                return m_faces;
            }

        protected:
            Matrix2Fr m_points;
            Matrix2Ir m_segments;

            Matrix2Fr m_vertices;
            Matrix3Ir m_faces;
    };
}
