#include "ElementWiseMaterial.h"

#include <algorithm>
#include <cassert>

VectorI ElementWiseMaterial::look_up_voxels(const VectorF& coords) const {
    const size_t dim = m_material_mesh->get_dim();
    const size_t length = coords.size();
    const size_t num_coords = length / dim;
    Eigen::Map<const MatrixFr> unflattened_coords(coords.data(), num_coords, dim);

    m_locator.locate(unflattened_coords);
    return m_locator.get_enclosing_voxels();
}

Float ElementWiseMaterial::get_material_tensor(
        size_t i, size_t j, size_t k, size_t l, VectorF coord) const {
    const VectorI voxel_ids = look_up_voxels(coord);
    assert(voxel_ids.size() > 0);
    const size_t voxel_id = voxel_ids[0];
    assert(voxel_id < m_materials.size());
    return m_materials[voxel_id]->get_material_tensor(i,j,k,l,coord);
}

MatrixF ElementWiseMaterial::strain_to_stress(
        const MatrixF& strain, VectorF coord) const {
    const VectorI voxel_ids = look_up_voxels(coord);
    assert(voxel_ids.size() > 0);
    const size_t voxel_id = voxel_ids[0];
    assert(voxel_id < m_materials.size());
    return m_materials[voxel_id]->strain_to_stress(strain, coord);
}

