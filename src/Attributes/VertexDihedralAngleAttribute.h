#pragma once

#include "MeshAttribute.h"

class VertexDihedralAngleAttribute : public MeshAttribute {
    public:
        VertexDihedralAngleAttribute(const std::string& name) : MeshAttribute(name) { }
        virtual ~VertexDihedralAngleAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
