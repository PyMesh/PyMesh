#pragma once

#include "MeshAttribute.h"

class VoxelVolumeAttribute : public MeshAttribute {
    public:
        VoxelVolumeAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VoxelVolumeAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        Float compute_tet_volume(Mesh& mesh, size_t voxel_idx);
        Float compute_hex_volume(Mesh& mesh, size_t voxel_idx);
};
