/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EigenSolver.h"
#include <sstream>
#include <Core/Exception.h>

using namespace PyMesh;

void EigenSolver::compute(const MatrixF& matrix) {
    m_solver.compute(matrix);
    m_eigen_values = m_solver.eigenvalues().real();
    m_eigen_vectors = m_solver.eigenvectors().real();
}

void EigenSolver::compute_batch_symmetric(size_t dim, VectorF matrices) {
    if (dim == 2) {
        compute_batch_symmetric_2x2(matrices);
    } else if (dim == 3) {
        compute_batch_symmetric_3x3(matrices);
    } else {
        std::stringstream err_msg;
        err_msg << "EigenSolver does not support " << dim << "D matrix yet.";
        throw NotImplementedError(err_msg.str());
    }
}

void EigenSolver::compute_batch_symmetric_2x2(const VectorF& matrices) {
    const size_t dim = 2;
    const size_t flatten_size = 3;
    const size_t num_matrices = matrices.size() / flatten_size;
    m_eigen_values = VectorF(num_matrices * dim);
    m_eigen_vectors = MatrixF(num_matrices * dim, dim);
    for (size_t i=0; i<num_matrices; i++) {
        const VectorF& entries = matrices.segment(i*flatten_size, flatten_size);
        MatrixF M(dim, dim);
        size_t base_idx = i*flatten_size;
        M << matrices[base_idx  ], matrices[base_idx+2],
             matrices[base_idx+2], matrices[base_idx+1],
        m_solver.compute(M);
        m_eigen_values.segment(i*dim, dim) = m_solver.eigenvalues().real();
        m_eigen_vectors.block(i*dim, 0, dim, dim) =
            m_solver.eigenvectors().real();
    }
}

void EigenSolver::compute_batch_symmetric_3x3(const VectorF& matrices) {
    const size_t dim = 3;
    const size_t flatten_size = 6;
    const size_t num_matrices = matrices.size() / flatten_size;
    m_eigen_values = VectorF(num_matrices * dim);
    m_eigen_vectors = MatrixF(num_matrices * dim, dim);
    for (size_t i=0; i<num_matrices; i++) {
        const VectorF& entries = matrices.segment(i*flatten_size, flatten_size);
        MatrixF M(dim, dim);
        size_t base_idx = i*flatten_size;
        M << matrices[base_idx  ], matrices[base_idx+5], matrices[base_idx+4],
             matrices[base_idx+5], matrices[base_idx+1], matrices[base_idx+3],
             matrices[base_idx+4], matrices[base_idx+3], matrices[base_idx+2];
        m_solver.compute(M);
        m_eigen_values.segment(i*dim, dim) = m_solver.eigenvalues().real();
        m_eigen_vectors.block(i*dim, 0, dim, dim) =
            m_solver.eigenvectors().real();
    }
}

