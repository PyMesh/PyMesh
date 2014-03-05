#pragma once

#include <string>

#include "MeshAttribute.h"

class VertexGaussianCurvatureAttribute : public MeshAttribute {
    public:
        VertexGaussianCurvatureAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~VertexGaussianCurvatureAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        VectorF compute_face_angles(const Mesh& mesh, size_t face_idx);
};
