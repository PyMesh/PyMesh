/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class VertexVolumeAttribute : public MeshAttribute {
    public:
        VertexVolumeAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexVolumeAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        VectorF& get_voxel_volumes(Mesh& mesh);
};

}
