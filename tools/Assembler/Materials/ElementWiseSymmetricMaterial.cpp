/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ElementWiseSymmetricMaterial.h"

#include <cassert>
#include <sstream>

#include <Core/Exception.h>

#include "Material.h"

using namespace PyMesh;

ElementWiseSymmetricMaterial::ElementWiseSymmetricMaterial(
        Float density, MeshPtr material_mesh,
        const std::string& matrix_field_name) :
    ElementWiseMaterial(density, material_mesh),
    m_matrix_field_name(matrix_field_name) {
        if (!m_material_mesh->has_attribute(m_matrix_field_name)) {
            std::stringstream err_msg;
            err_msg << "Cannot find attribute: " << m_matrix_field_name;
            throw RuntimeError(err_msg.str());
        }
        update();
}

void ElementWiseSymmetricMaterial::update() {
    const size_t dim = m_material_mesh->get_dim();
    if (dim == 2) {
        update_2D();
    } else if (dim == 3) {
        update_3D();
    } else {
        std::stringstream err_msg;
        err_msg << "Unknow dimention: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

void ElementWiseSymmetricMaterial::update_2D() {
    const size_t num_entries_per_matrix = 9;
    const size_t num_faces = m_material_mesh->get_num_faces();
    const VectorF& matrix_field = m_material_mesh->get_attribute(m_matrix_field_name);
    if (matrix_field.size() != num_faces * num_entries_per_matrix) {
        std::stringstream err_msg;
        err_msg << "Symmetric material matrix field (" << m_matrix_field_name
            << ") has invalid length: " << matrix_field.size() << " != "
            << num_faces << " * " << num_entries_per_matrix;
        throw RuntimeError(err_msg.str());
    }

    m_materials.resize(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        const VectorF& entries = matrix_field.segment(
                i * num_entries_per_matrix, num_entries_per_matrix);
        Eigen::Map<const MatrixF> material_matrix(entries.data(), 3, 3);
        m_materials[i] = Material::create_symmetric(m_density, material_matrix);
    }
}

void ElementWiseSymmetricMaterial::update_3D() {
    const size_t num_entries_per_matrix = 36;
    const size_t num_voxels = m_material_mesh->get_num_voxels();
    const VectorF& matrix_field = m_material_mesh->get_attribute(m_matrix_field_name);
    if (matrix_field.size() != num_voxels * num_entries_per_matrix) {
        std::stringstream err_msg;
        err_msg << "Symmetric material matrix field (" << m_matrix_field_name
            << ") has invalid length: " << matrix_field.size() << " != "
            << num_voxels << " * " << num_entries_per_matrix;
        throw RuntimeError(err_msg.str());
    }

    m_materials.resize(num_voxels);
    for (size_t i=0; i<num_voxels; i++) {
        const VectorF& entries = matrix_field.segment(
                i * num_entries_per_matrix, num_entries_per_matrix);
        Eigen::Map<const MatrixF> material_matrix(entries.data(), 6, 6);
        m_materials[i] = Material::create_symmetric(m_density, material_matrix);
    }
}

