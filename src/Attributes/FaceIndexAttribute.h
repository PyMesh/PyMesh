#pragma once

#include "MeshAttribute.h"

class FaceIndexAttribute : public MeshAttribute {
    public:
        FaceIndexAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceIndexAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
