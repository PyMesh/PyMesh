/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "Material.h"

namespace PyMesh {

class UniformMaterial : public Material {
    public:
        /**
         * Material tensor is represented by dim^2 x dim^2 matrix.
         * Its format is dim x dim arrays of dim x dim matrices.
         */
        UniformMaterial(Float density, const MatrixF& material_tensor);

        virtual Float get_material_tensor(
                size_t i, size_t j, size_t k, size_t l, VectorF coord) const;

        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const;

        virtual Float get_density(VectorF coord) const {
            return m_density;
        }

        virtual Float get_density() const {
            return m_density;
        }

        virtual size_t get_dim() const { return m_dim; }

    protected:
        UniformMaterial() {}

    protected:
        size_t m_dim;
        Float m_density;
        MatrixF m_material_tensor;
};

}
