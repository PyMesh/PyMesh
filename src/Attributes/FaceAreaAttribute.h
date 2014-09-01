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
        Float compute_quad_area(Mesh& mesh, size_t face_idx);

        Float compute_triangle_area(
                const Vector3F& v0, const Vector3F& v1, const Vector3F& v2) {
            return ((v2 - v0).cross(v1 - v0)).norm() * 0.5;
        }
};
