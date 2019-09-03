/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceIncircleRadiusAttribute : public MeshAttribute {
    public:
        virtual ~FaceIncircleRadiusAttribute() = default;

    public:
        virtual void compute_from_mesh(Mesh& mesh) override;
};

}
