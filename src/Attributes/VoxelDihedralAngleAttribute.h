/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class Mesh;

class VoxelDihedralAngleAttribute : public MeshAttribute {
    public:
        VoxelDihedralAngleAttribute(const std::string& name)
            : MeshAttribute(name) {}
        virtual ~VoxelDihedralAngleAttribute()=default;

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
