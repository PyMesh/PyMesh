/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceAspectRatioAttribute : public MeshAttribute {
    public:
        FaceAspectRatioAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceAspectRatioAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
