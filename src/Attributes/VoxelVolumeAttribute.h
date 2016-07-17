/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class VoxelVolumeAttribute : public MeshAttribute {
    public:
        VoxelVolumeAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VoxelVolumeAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        Float compute_signed_tet_volume(Mesh& mesh, size_t voxel_idx);
        Float compute_signed_hex_volume(Mesh& mesh, size_t voxel_idx);
};

}
