#include "ElementWiseIsotropicMaterial.h"

#include <cassert>
#include <sstream>

#include <Core/Exception.h>

#include "Material.h"

ElementWiseIsotropicMaterial::ElementWiseIsotropicMaterial(
        Float density, MeshPtr material_mesh,
        const std::string& young_field_name,
        const std::string& poisson_field_name) :
    ElementWiseMaterial(density, material_mesh),
    m_young_field_name(young_field_name),
    m_poisson_field_name(poisson_field_name) {
        if (!m_material_mesh->has_attribute(m_young_field_name)) {
            std::stringstream err_msg;
            err_msg << "Cannot find attribute: " << m_young_field_name;
            throw RuntimeError(err_msg.str());
        }
        if (!m_material_mesh->has_attribute(m_poisson_field_name)) {
            std::stringstream err_msg;
            err_msg << "Cannot find attribute: " << m_young_field_name;
            throw RuntimeError(err_msg.str());
        }
        update();
}

Float ElementWiseIsotropicMaterial::get_material_tensor(
        size_t i, size_t j, size_t k, size_t l, VectorF coord) const {
    const VectorI voxel_ids = look_up_voxels(coord);
    assert(voxel_ids.size() > 0);
    const size_t voxel_id = voxel_ids[0];
    assert(voxel_id < m_materials.size());
    return m_materials[voxel_id]->get_material_tensor(i,j,k,l,coord);
}

MatrixF ElementWiseIsotropicMaterial::strain_to_stress(
        const MatrixF& strain, VectorF coord) const {
    const VectorI voxel_ids = look_up_voxels(coord);
    assert(voxel_ids.size() > 0);
    const size_t voxel_id = voxel_ids[0];
    assert(voxel_id < m_materials.size());
    return m_materials[voxel_id]->strain_to_stress(strain, coord);
}

void ElementWiseIsotropicMaterial::update() {
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

void ElementWiseIsotropicMaterial::update_2D() {
    const size_t num_faces = m_material_mesh->get_num_faces();
    const VectorF& young = m_material_mesh->get_attribute(m_young_field_name);
    const VectorF& poisson = m_material_mesh->get_attribute(m_poisson_field_name);
    assert(young.size() == num_faces);
    assert(poisson.size() == num_faces);

    m_materials.resize(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        m_materials[i] = Material::create_isotropic(2, m_density,
                young[i], poisson[i]);
    }
}

void ElementWiseIsotropicMaterial::update_3D() {
    const size_t num_voxels = m_material_mesh->get_num_voxels();
    const VectorF& young = m_material_mesh->get_attribute(m_young_field_name);
    const VectorF& poisson = m_material_mesh->get_attribute(m_poisson_field_name);
    assert(young.size() == num_voxels);
    assert(poisson.size() == num_voxels);

    m_materials.resize(num_voxels);
    for (size_t i=0; i<num_voxels; i++) {
        m_materials[i] = Material::create_isotropic(3, m_density,
                young[i], poisson[i]);
    }
}

