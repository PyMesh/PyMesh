/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceCircumRadiusAttribute : public MeshAttribute {
    public:
        FaceCircumRadiusAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceCircumRadiusAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
