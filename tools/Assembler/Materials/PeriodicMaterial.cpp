/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PeriodicMaterial.h"
#include <sstream>
#include <Core/Exception.h>

using namespace PyMesh;

PeriodicMaterial::PeriodicMaterial(Material::Ptr mat1, Material::Ptr mat2,
        VectorF axis, Float period, Float ratio, Float phase) :
    m_axis(axis), m_period(period), m_ratio(ratio), m_phase(phase) {
        assert(mat1->get_dim() == mat2->get_dim());
        assert(m_axis.size() == mat1->get_dim());
        m_materials.push_back(mat1);
        m_materials.push_back(mat2);
        m_axis.normalize();
        check_validity();
}

Float PeriodicMaterial::get_material_tensor(
        size_t i, size_t j, size_t k, size_t l, VectorF coord) const {
    size_t mat_idx = choose_material(coord);
    return m_materials[mat_idx]->get_material_tensor(i, j, k, l, coord);
}

MatrixF PeriodicMaterial::strain_to_stress(
        const MatrixF& strain, VectorF coord) const {
    size_t mat_idx = choose_material(coord);
    return m_materials[mat_idx]->strain_to_stress(strain, coord);
}

Float PeriodicMaterial::get_density(VectorF coord) const {
    size_t mat_idx = choose_material(coord);
    return m_materials[mat_idx]->get_density(coord);
}

Float PeriodicMaterial::get_density() const {
    throw RuntimeError("Please specify a coordinate to get mixed material density.");
}

size_t PeriodicMaterial::choose_material(VectorF coord) const {
    size_t num_materials = m_materials.size();
    Float proj = coord.dot(m_axis) - m_phase * m_period;
    Float int_part;
    Float frac_part = modf(proj / m_period, &int_part);
    if (frac_part < 0) {
        frac_part = 1.0 + frac_part;
    }

    if (frac_part <= m_ratio) {
        return 0;
    } else {
        return 1;
    }
}

void PeriodicMaterial::check_validity() const {
    Float axis_length = m_axis.norm();
    if (axis_length < 0.9) {
        std::stringstream err_msg;
        err_msg << "Axis of periodicity (" << m_axis << ") is denerated.";
        throw RuntimeError(err_msg.str());
    }

    if (m_ratio > 1.0 || m_ratio < 0.0) {
        std::stringstream err_msg;
        err_msg << "Material ratio (" << m_ratio << ") is not in [0,1]";
        throw RuntimeError(err_msg.str());
    }

    if (m_phase > 1.0 || m_phase < -1.0) {
        std::stringstream err_msg;
        err_msg << "Starting phase (" << m_phase << ") is not in [-1,1]";
        throw RuntimeError(err_msg.str());
    }
}
