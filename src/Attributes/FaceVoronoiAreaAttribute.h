/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshAttribute.h"

namespace PyMesh {

/**
 * Compute the Voronoi area of each corner of the face.
 */
class FaceVoronoiAreaAttribute : public MeshAttribute {
    public:
        FaceVoronoiAreaAttribute(const std::string& name) : MeshAttribute(name) {}
        virtual ~FaceVoronoiAreaAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh);

    private:
        VectorF compute_triangle_voronoi_area(Mesh& mesh, size_t face_idx);
};

}
