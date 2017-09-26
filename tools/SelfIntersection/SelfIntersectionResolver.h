/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

class SelfIntersectionResolver {
    public:
        typedef std::shared_ptr<SelfIntersectionResolver> Ptr;
        static Ptr create(const std::string& engine_name);

    public:
        virtual ~SelfIntersectionResolver() = default;

    public:
        void set_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
            m_vertices = vertices;
            m_faces = faces;
        }

        virtual void run() {
            throw NotImplementedError(
                    "Resolving self-interesection is not implemented");
        }

        const MatrixFr get_vertices() const { return m_vertices; }
        const MatrixIr get_faces() const { return m_faces; }
        const VectorI  get_face_sources() const { return m_face_sources; }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI m_face_sources;
};

}
