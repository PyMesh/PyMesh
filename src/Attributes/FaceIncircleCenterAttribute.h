/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceIncircleCenterAttribute : public MeshAttribute {
    public:
        FaceIncircleCenterAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceIncircleCenterAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
}
