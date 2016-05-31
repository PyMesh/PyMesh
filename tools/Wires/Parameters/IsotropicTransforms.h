/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class IsotropicTransforms {
    public:
        IsotropicTransforms(size_t dim) : m_dim(dim) {
            initialize();
        }

        MatrixF fit(const VectorF& from_dir, const VectorF& to_dir) const;

    private:
        void initialize();
        void initialize_2D_reflections();
        void initialize_2D_rotations();
        void initialize_3D_reflections();
        void initialize_3D_rotations();

    private:
        size_t m_dim;

        // Any isotropic transformation can be decomposed into a rotation
        // followed by a reflection then followed by a rotation.
        std::vector<MatrixF> m_reflections;
        std::vector<MatrixF> m_rotations;
};

}
