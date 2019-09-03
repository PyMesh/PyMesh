/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class VertexVolumeAttribute : public MeshAttribute {
    public:
        virtual ~VertexVolumeAttribute() = default;

    public:
        virtual void compute_from_mesh(Mesh& mesh) override;

    private:
        VectorF& get_voxel_volumes(Mesh& mesh);
};

}
