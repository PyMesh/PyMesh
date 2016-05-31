/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VoxelIndexAttribute.h"

#include <Mesh.h>

using namespace PyMesh;

void VoxelIndexAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    VectorF& indices = m_values;
    indices = VectorF::LinSpaced(num_voxels, 0, num_voxels-1);
}
