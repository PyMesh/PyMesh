#pragma once

#include "MeshAttribute.h"

class EdgeDihedralAngleAttribute : public MeshAttribute {
    public:
        EdgeDihedralAngleAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~EdgeDihedralAngleAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
