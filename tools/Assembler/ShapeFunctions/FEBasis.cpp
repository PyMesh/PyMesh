/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FEBasis.h"

using namespace PyMesh;

FEBasis::FEBasis(FEBasis::FEMeshPtr mesh) {
    m_shape_func = ShapeFunction::create(mesh, "linear");
    m_integrator = Integrator::create(mesh, m_shape_func);
}

Float FEBasis::evaluate_func(size_t elem_idx,
        size_t local_func_idx, const VectorF& coord) {
    return m_shape_func->evaluate_func(elem_idx, local_func_idx, coord);
}

VectorF FEBasis::evaluate_grad(size_t elem_idx,
        size_t local_func_idx, const VectorF& coord) {
    return m_shape_func->evaluate_grad(elem_idx, local_func_idx, coord);
}

Float FEBasis::integrate_func_func(size_t elem_idx,
        size_t local_func_i, size_t local_func_j) {
    return m_integrator->integrate_func(elem_idx, local_func_i, local_func_j);
}

Float FEBasis::integrate_grad_grad(size_t elem_idx,
        size_t local_func_i, size_t local_func_j) {
    return m_integrator->integrate_grad(elem_idx, local_func_i, local_func_j);
}

Float FEBasis::integrate_grad_C_grad(size_t elem_idx,
        size_t local_func_i, size_t local_func_j, const MatrixF& C) {
    return m_integrator->integrate_grad_C(elem_idx,
            local_func_i, local_func_j, C);
}

MatrixF FEBasis::integrate_material_contraction(size_t elem_idx,
        size_t local_func_i, size_t local_func_j,
        const MaterialPtr material) {
    return m_integrator->integrate_material_contraction(elem_idx,
            local_func_i, local_func_j, material);
}
