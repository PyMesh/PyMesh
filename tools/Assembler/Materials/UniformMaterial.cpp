/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "UniformMaterial.h"
#include <Core/Exception.h>

using namespace PyMesh;

UniformMaterial::UniformMaterial(
        Float density, const MatrixF& material_tensor) :
        m_density(density), m_material_tensor(material_tensor) {
    const size_t rows = m_material_tensor.rows();
    if (rows == 9) {
        m_dim = 3;
    } else if (rows == 4) {
        m_dim = 2;
    } else {
        throw NotImplementedError("Material tensor size not supported.");
    }
}

Float UniformMaterial::get_material_tensor(
        size_t i, size_t j, size_t k, size_t l, VectorF coord) const {
    size_t row = i * m_dim + k;
    size_t col = j * m_dim + l;
    return m_material_tensor.coeff(row, col);
}

MatrixF UniformMaterial::strain_to_stress(const MatrixF& strain,
        VectorF coord) const {
    const size_t rows = strain.rows();
    const size_t cols = strain.cols();
    MatrixF stress = MatrixF::Zero(rows, cols);
    for (size_t i=0; i<rows; i++) {
        for (size_t j=0; j<cols; j++) {
            MatrixF C = m_material_tensor.block(
                    i*rows, j*cols, rows, cols);
            stress(i, j) = C.cwiseProduct(strain).sum();
        }
    }
    return stress;
}
