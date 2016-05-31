/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

/**
 * Edge length will be a face attribute with x Float associated with each face,
 * where x is the number vertices per face.
 * Yes, it is wasteful on memory.
 */
class EdgeSquaredLengthAttribute : public MeshAttribute {
    public:
        EdgeSquaredLengthAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~EdgeSquaredLengthAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        VectorF compute_edge_squared_length_on_face(Mesh& mesh, size_t face_idx);
};

}
