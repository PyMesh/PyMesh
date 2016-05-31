/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceIndexAttribute : public MeshAttribute {
    public:
        FaceIndexAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceIndexAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
