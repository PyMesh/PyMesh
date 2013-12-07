#pragma once

#include "MeshAttribute.h"

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

    private:
        VectorF compute_edge_length_on_face(Mesh& mesh, size_t face_idx);
};
