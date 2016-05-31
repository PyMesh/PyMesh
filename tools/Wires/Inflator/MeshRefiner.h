/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <Core/EigenTypedef.h>
#include <MeshUtils/Subdivision.h>

namespace PyMesh {

class MeshRefiner {
    public:
        MeshRefiner(const std::string& method);

        void refine(const MatrixFr& vertices, const MatrixIr& faces,
                size_t num_iterations) {
            m_refiner->subdivide(vertices, faces, num_iterations);
        }

        MatrixFr get_vertices() const { return m_refiner->get_vertices(); }
        MatrixIr get_faces() const { return m_refiner->get_faces(); }

        VectorI update_face_sources(const VectorI& face_sources) const;
        MatrixFr update_vertex_field(const MatrixFr& field) const;

    private:
        Subdivision::Ptr m_refiner;
};

}
