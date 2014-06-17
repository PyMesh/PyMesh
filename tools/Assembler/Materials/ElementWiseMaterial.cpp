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
