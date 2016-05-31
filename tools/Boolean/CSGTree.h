/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class CSGTree {
    public:
        typedef std::shared_ptr<CSGTree> Ptr;
        static Ptr create(const std::string& engine_name);
        static Ptr create_leaf(const std::string& engine_name,
                const MatrixFr& vertices,
                const MatrixIr& faces);

    public:
        CSGTree() {}
        CSGTree(const MatrixFr& vertices, const MatrixIr& faces) :
            m_vertices(vertices), m_faces(faces) {}
        virtual ~CSGTree() {}

    public:
        void set_operand_1(Ptr tree) { m_tree_1 = tree; }
        void set_operand_2(Ptr tree) { m_tree_2 = tree; }

    public:
        virtual void compute_union() =0;
        virtual void compute_intersection() =0;
        virtual void compute_difference() =0;
        virtual void compute_symmetric_difference() =0;
        virtual VectorI get_face_sources() const {
            return VectorI::Zero(0);
        };
        virtual VectorI get_mesh_sources() const {
            return VectorI::Zero(0);
        }
        virtual MatrixFr get_vertices() const { return m_vertices; }
        virtual MatrixIr get_faces() const { return m_faces; }
        virtual size_t get_num_vertices() const { return m_vertices.rows(); }
        virtual size_t get_num_faces() const { return m_faces.rows(); }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;

        Ptr m_tree_1;
        Ptr m_tree_2;
};

}
