#pragma once

#include <string>

#include "MeshAttribute.h"

class VertexMeanCurvatureAttribute : public MeshAttribute {
    public:
        VertexMeanCurvatureAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexMeanCurvatureAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        VectorF compute_laplacian_vectors(Mesh& mesh);
        VectorF compute_vertex_normals(Mesh& mesh);
};
