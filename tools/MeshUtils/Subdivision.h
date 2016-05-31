/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <Core/EigenTypedef.h>
#include <Math/ZSparseMatrix.h>

namespace PyMesh {
class Subdivision {
    public:
        typedef std::shared_ptr<Subdivision> Ptr;

        static Ptr create(const std::string& type);

    public:
        virtual ~Subdivision() {}

        virtual void subdivide(MatrixFr vertices, MatrixIr faces,
                size_t num_iterations)=0;

        /**
         * Each iteration of subdivision can be thought of as applying a linear
         * transfermation on the vertex coordinates.  This method compute and
         * returns such transformation matrices for each iteration.
         */
        virtual const std::vector<ZSparseMatrix>& get_subdivision_matrices() const=0;

    public:
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_face_indices() const { return m_face_indices; }

        size_t get_num_vertices() const { return m_vertices.rows(); }
        size_t get_num_faces() const { return m_faces.rows(); }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_face_indices;
};
}
