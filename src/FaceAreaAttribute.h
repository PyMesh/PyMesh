#pragma once

#include "MeshAttribute.h"

class FaceAreaAttribute : public MeshAttribute {
    public:
        FaceAreaAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceAreaAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        Float compute_triangle_area(Mesh& mesh, size_t face_idx);
};
