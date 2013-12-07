#pragma once

#include "MeshAttribute.h"

class VertexAreaAttribute : public MeshAttribute {
    public:
        VertexAreaAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexAreaAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        VectorF& get_face_areas(Mesh& mesh);
};
