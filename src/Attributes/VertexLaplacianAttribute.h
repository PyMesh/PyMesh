/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include "MeshAttribute.h"

namespace PyMesh {

class VertexLaplacianAttribute : public MeshAttribute {
    public:
        VertexLaplacianAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexLaplacianAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        VectorF compute_cotan_weights(const VectorF& v0,
                const VectorF& v1, const VectorF& v2);
};

}
