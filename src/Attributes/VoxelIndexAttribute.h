#pragma once

#include "MeshAttribute.h"

class VoxelIndexAttribute : public MeshAttribute {
    public:
        VoxelIndexAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VoxelIndexAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
