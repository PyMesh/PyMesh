/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceCircumCenterAttribute : public MeshAttribute {
    public:
        FaceCircumCenterAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceCircumCenterAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
