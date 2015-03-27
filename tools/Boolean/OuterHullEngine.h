/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

class OuterHullEngine {
    public:
        typedef std::shared_ptr<OuterHullEngine> Ptr;
        static Ptr create(const std::string& engine_name);

    public:
        virtual ~OuterHullEngine() {}

    public:
        virtual void run()=0;

        void set_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
            m_vertices = vertices;
            m_faces = faces;
        }

        const MatrixFr get_vertices() const { return m_vertices; }
        const MatrixIr get_faces() const { return m_faces; }
        const MatrixFr get_interior_vertices() const { return m_interior_vertices; }
        const MatrixIr get_interior_faces() const { return m_interior_faces; }
        const VectorF  get_debug_field() const { return m_debug; }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixFr m_interior_vertices;
        MatrixIr m_interior_faces;
        VectorF  m_debug;
};
