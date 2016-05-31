/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceNormalAttribute : public MeshAttribute {
    public:
        FaceNormalAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceNormalAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        Vector3F compute_triangle_normal(Mesh& mesh, size_t i);
        Vector3F compute_quad_normal(Mesh& mesh, size_t i);
};

}
