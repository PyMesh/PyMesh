#include "PeriodicMaterial.h"
#include <Core/Exception.h>

PeriodicMaterial::PeriodicMaterial(Material::Ptr mat1, Material::Ptr mat2,
        VectorF axis, Float period, Float ratio) :
    m_axis(axis), m_period(period), m_ratio(ratio) {
        m_materials.push_back(mat1);
        m_materials.push_back(mat2);
        m_axis.normalize();
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
    Float proj = coord.dot(m_axis);
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
