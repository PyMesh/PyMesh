#pragma once

#include "MeshAttribute.h"

class VoxelCentroidAttribute : public MeshAttribute {
    public:
        VoxelCentroidAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VoxelCentroidAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
