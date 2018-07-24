/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

class FaceFrameAttribute : public MeshAttribute {
    public:
        FaceFrameAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceFrameAttribute() = default;

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    protected:
        void compute_2D_frame_field(Mesh& mesh);
        void compute_3D_frame_field(Mesh& mesh);
};

}
