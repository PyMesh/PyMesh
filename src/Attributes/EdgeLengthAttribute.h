/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

/**
 * Edge length will be a face attribute with x Float associated with each face,
 * where x is the number vertices per face.
 * Yes, it is wasteful on memory.
 */
class EdgeLengthAttribute : public MeshAttribute {
    public:
        virtual ~EdgeLengthAttribute() = default;

    public:
        virtual void compute_from_mesh(Mesh& mesh) override;
};

}
