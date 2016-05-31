/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {
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
        const VectorI  get_outer_hull_layers() const { return m_layers; }
        const VectorI  get_face_is_flipped() const { return m_face_is_flipped; }
        const VectorI  get_ori_face_indices() const { return m_ori_face_indices; }

    protected:
        void remove_isolated_vertices();

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_layers;
        VectorI  m_face_is_flipped;
        VectorI  m_ori_face_indices;
};
}
