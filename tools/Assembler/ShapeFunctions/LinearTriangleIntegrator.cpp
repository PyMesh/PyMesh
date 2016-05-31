/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "LinearTriangleIntegrator.h"

using namespace PyMesh;

LinearTriangleIntegrator::LinearTriangleIntegrator(FEMeshPtr mesh, ShapeFuncPtr shape_func)
    : m_mesh(mesh), m_shape_func(shape_func) { }

Float LinearTriangleIntegrator::integrate_func(size_t elem_idx,
        size_t local_func_i, size_t local_func_j) {
    // Note the following magic number comes from evaluating
    // (shape_func_i * shape_func_j) at Gaussian quadrature points over
    // triangle.
    const Float coeff_ii = 1.0 / 6.0;
    const Float coeff_ij = 1.0 / 12.0;
    const Float vol = m_mesh->getElementVolume(elem_idx);
    if (local_func_i == local_func_j) {
        return coeff_ii * vol;
    } else {
        return coeff_ij * vol;
    }
}

Float LinearTriangleIntegrator::integrate_grad(size_t elem_idx,
        size_t local_func_i, size_t local_func_j) {
    const Vector3F coord(1.0/3.0, 1.0/3.0, 1.0/3.0);
    const VectorF grad_i = m_shape_func->evaluate_grad(
            elem_idx, local_func_i, coord);
    const VectorF grad_j = m_shape_func->evaluate_grad(
            elem_idx, local_func_j, coord);
    const Float vol = m_mesh->getElementVolume(elem_idx);
    return grad_i.dot(grad_j) * vol;
}

Float LinearTriangleIntegrator::integrate_grad_C(size_t elem_idx,
        size_t local_func_i, size_t local_func_j, const MatrixF& C) {
    const Vector3F coord(1.0/3.0, 1.0/3.0, 1.0/3.0);
    const VectorF grad_i = m_shape_func->evaluate_grad(
            elem_idx, local_func_i, coord);
    const VectorF grad_j = m_shape_func->evaluate_grad(
            elem_idx, local_func_j, coord);
    const Float vol = m_mesh->getElementVolume(elem_idx);
    return grad_i.dot(C * grad_j) * vol;
}

MatrixF LinearTriangleIntegrator::integrate_material_contraction(
        size_t elem_idx, size_t local_func_i, size_t local_func_j,
        const Material::Ptr material) {
    MatrixF coeff(2, 2);
    VectorF coord = m_mesh->getElementCenter(elem_idx);

    for (size_t i=0; i<2; i++) {
        for (size_t k=0; k<2; k++) {
            MatrixF C(2, 2);

            C(0, 0) = 0.5 * (
                    material->get_material_tensor(i, 0, k, 0, coord) +
                    material->get_material_tensor(i, 0, 0, k, coord));
            C(0, 1) = 0.5 * (
                    material->get_material_tensor(i, 0, k, 1, coord) +
                    material->get_material_tensor(i, 0, 1, k, coord));

            C(1, 0) = 0.5 * (
                    material->get_material_tensor(i, 1, k, 0, coord) +
                    material->get_material_tensor(i, 1, 0, k, coord));
            C(1, 1) = 0.5 * (
                    material->get_material_tensor(i, 1, k, 1, coord) +
                    material->get_material_tensor(i, 1, 1, k, coord));

            coeff(i, k) = integrate_grad_C(elem_idx, local_func_i, local_func_j, C);
        }
    }

    return coeff;
}

