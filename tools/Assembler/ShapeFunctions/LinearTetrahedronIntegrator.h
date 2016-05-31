/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "Integrator.h"

namespace PyMesh {

class LinearTetrahedronIntegrator : public Integrator {
    public:
        LinearTetrahedronIntegrator(FEMeshPtr mesh, ShapeFuncPtr shape_func);

    public:
        virtual Float integrate_func(size_t elem_idx,
                size_t local_func_i, size_t local_func_j);
        virtual Float integrate_grad(size_t elem_idx,
                size_t local_func_i, size_t local_func_j);
        virtual Float integrate_grad_C(size_t elem_idx,
                size_t local_func_i, size_t local_func_j, const MatrixF& C);
        virtual MatrixF integrate_material_contraction(size_t elem_idx,
                size_t local_func_i, size_t local_func_j,
                const Material::Ptr material);

    private:
        FEMeshPtr m_mesh;
        ShapeFuncPtr m_shape_func;
};

}
