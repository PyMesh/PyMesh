/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceNormalAttribute : public MeshAttribute {
    public:
        virtual ~FaceNormalAttribute() = default;

    public:
        virtual void compute_from_mesh(Mesh& mesh) override;

    private:
        Vector3F compute_triangle_normal(Mesh& mesh, size_t i);
        Vector3F compute_quad_normal(Mesh& mesh, size_t i);
};

}
