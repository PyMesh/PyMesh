#pragma once

#include "MeshAttribute.h"

class VertexIndexAttribute : public MeshAttribute {
    public:
        VertexIndexAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexIndexAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};
