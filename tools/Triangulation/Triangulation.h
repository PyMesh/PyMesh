/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <memory>
#include <vector>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {
    class Triangulation {
        public:
            typedef std::shared_ptr<Triangulation> Ptr;
            static Ptr create(const std::string& engine_name);
            static bool supports(const std::string& engine_name);
            static std::vector<std::string> get_available_engines();

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

            virtual void refine(const MatrixFr& metric) {
                throw NotImplementedError(
                        "Refinement algorithm is not implemented");
            }

            void set_vertices(const Matrix2Fr& vertices) {
                m_vertices = vertices;
            }

            void set_faces(const Matrix3Ir& faces) {
                m_faces = faces;
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
