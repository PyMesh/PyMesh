/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class VoxelCentroidAttribute : public MeshAttribute {
    public:
        VoxelCentroidAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VoxelCentroidAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
