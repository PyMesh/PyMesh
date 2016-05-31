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
        EdgeLengthAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~EdgeLengthAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);
};

}
