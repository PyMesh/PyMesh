#pragma once
#include <string>
#include <memory>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

class Subdivision {
    public:
        typedef std::shared_ptr<Subdivision> Ptr;

        static Ptr create(const std::string& type);

    public:
        virtual ~Subdivision() {}

        virtual void subdivide(MatrixFr vertices, MatrixIr faces,
                size_t num_iterations) {
            throw NotImplementedError("subdivide method should be implemented by subclasses");
        };

    public:
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_face_indices() const { return m_face_indices; }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_face_indices;
};
