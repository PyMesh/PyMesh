/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class VoxelInradiusAttribute : public MeshAttribute {
    public:
        VoxelInradiusAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VoxelInradiusAttribute() = default;

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
