/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include "MeshAttribute.h"

namespace PyMesh {

class VertexNormalAttribute : public MeshAttribute {
    public:
        VertexNormalAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexNormalAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        void compute_vertex_normals_from_face(Mesh& mesh);
        void compute_vertex_normals_from_edge(Mesh& mesh);
        const VectorF& get_attribute(Mesh& mesh, const std::string& attr_name);
};

}
