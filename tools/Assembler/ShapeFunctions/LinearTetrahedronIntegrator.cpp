#include "LinearTetrahedronIntegrator.h"
#include <iostream>

LinearTetrahedronIntegrator::LinearTetrahedronIntegrator(FEMeshPtr mesh, ShapeFuncPtr shape_func)
    : m_mesh(mesh), m_shape_func(shape_func) { }

Float LinearTetrahedronIntegrator::integrate_func(size_t elem_idx,
        size_t local_func_i, size_t local_func_j) {
    // Note the following magic number comes from evaluating
    // (shape_func_i * shape_func_j) at Gaussian quadrature points over
    // tetrahedron.
    const Float coeff_ii = 1.0 / 10.0;
    const Float coeff_ij = 1.0 / 20.0;
    const Float vol = m_mesh->getElementVolume(elem_idx);
    if (local_func_i == local_func_j) {
        return coeff_ii * vol;
    } else {
        return coeff_ij * vol;
    }
}

Float LinearTetrahedronIntegrator::integrate_grad(size_t elem_idx,
        size_t local_func_i, size_t local_func_j) {
    const Vector4F coord(1.0/4.0, 1.0/4.0, 1.0/4.0, 1.0/4.0);
    const VectorF grad_i = m_shape_func->evaluate_grad(
            elem_idx, local_func_i, coord);
    const VectorF grad_j = m_shape_func->evaluate_grad(
            elem_idx, local_func_j, coord);
    const Float vol = m_mesh->getElementVolume(elem_idx);
    return grad_i.dot(grad_j) * vol;
}

Float LinearTetrahedronIntegrator::integrate_grad_C(size_t elem_idx,
        size_t local_func_i, size_t local_func_j, const MatrixF& C) {
    const Vector4F coord(1.0/4.0, 1.0/4.0, 1.0/4.0, 1.0/4.0);
    const VectorF grad_i = m_shape_func->evaluate_grad(
            elem_idx, local_func_i, coord);
    const VectorF grad_j = m_shape_func->evaluate_grad(
            elem_idx, local_func_j, coord);
    const Float vol = m_mesh->getElementVolume(elem_idx);
    return grad_i.dot(C * grad_j) * vol;
}

MatrixF LinearTetrahedronIntegrator::integrate_material_contraction(
        size_t elem_idx, size_t local_func_i, size_t local_func_j,
        const Material::Ptr material) {
    MatrixF coeff(3, 3);

    for (size_t i=0; i<3; i++) {
        for (size_t k=0; k<3; k++) {
            MatrixF C(3, 3);

            C(0, 0) = 0.5 * (
                    material->get_material_tensor(i, 0, k, 0) +
                    material->get_material_tensor(i, 0, 0, k));
            C(0, 1) = 0.5 * (
                    material->get_material_tensor(i, 0, k, 1) +
                    material->get_material_tensor(i, 0, 1, k));
            C(0, 2) = 0.5 * (
                    material->get_material_tensor(i, 0, k, 2) +
                    material->get_material_tensor(i, 0, 2, k));

            C(1, 0) = 0.5 * (
                    material->get_material_tensor(i, 1, k, 0) +
                    material->get_material_tensor(i, 1, 0, k));
            C(1, 1) = 0.5 * (
                    material->get_material_tensor(i, 1, k, 1) +
                    material->get_material_tensor(i, 1, 1, k));
            C(1, 2) = 0.5 * (
                    material->get_material_tensor(i, 1, k, 2) +
                    material->get_material_tensor(i, 1, 2, k));

            C(2, 0) = 0.5 * (
                    material->get_material_tensor(i, 2, k, 0) +
                    material->get_material_tensor(i, 2, 0, k));
            C(2, 1) = 0.5 * (
                    material->get_material_tensor(i, 2, k, 1) +
                    material->get_material_tensor(i, 2, 1, k));
            C(2, 2) = 0.5 * (
                    material->get_material_tensor(i, 2, k, 2) +
                    material->get_material_tensor(i, 2, 2, k));

            coeff(i, k) = integrate_grad_C(elem_idx, local_func_i, local_func_j, C);
        }
    }

    return coeff;
}

