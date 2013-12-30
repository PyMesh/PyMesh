#pragma once

#include <string>

#include "MeshAttribute.h"

class VertexNormalAttribute : public MeshAttribute {
    public:
        VertexNormalAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexNormalAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        void compute_vertex_normals(Mesh& mesh);
        const VectorF& get_attribute(Mesh& mesh, const std::string& attr_name);
};
