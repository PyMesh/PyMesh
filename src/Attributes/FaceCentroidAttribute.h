#pragma once

#include "MeshAttribute.h"

class FaceCentroidAttribute : public MeshAttribute {
    public:
        FaceCentroidAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceCentroidAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
