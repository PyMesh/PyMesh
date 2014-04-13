#pragma once

#include "MeshAttribute.h"

class FaceCircumCenterAttribute : public MeshAttribute {
    public:
        FaceCircumCenterAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceCircumCenterAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
