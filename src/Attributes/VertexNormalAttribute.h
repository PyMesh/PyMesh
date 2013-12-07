#pragma once

#include "MeshAttribute.h"

class VertexNormalAttribute : public MeshAttribute {
    public:
        VertexNormalAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexNormalAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        void compute_vertex_normals(Mesh& mesh);
};
