#include "VoxelIndexAttribute.h"

#include <Mesh.h>

void VoxelIndexAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    VectorF& indices = m_values;
    indices = VectorF::LinSpaced(num_voxels, 0, num_voxels-1);
}
