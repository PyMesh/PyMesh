/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VoxelCentroidAttribute.h"

#include <Mesh.h>

using namespace PyMesh;

void VoxelCentroidAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();

    VectorF& centroids = m_values;
    centroids.resize(num_voxels*3);

    for (size_t i=0; i<num_voxels; i++) {
        VectorI voxel = mesh.get_voxel(i);

        Vector3F centroid = Vector3F::Zero();
        for (size_t j=0; j<vertex_per_voxel; j++) {
            centroid += mesh.get_vertex(voxel[j]);
        }
        centroid /= vertex_per_voxel;

        centroids.segment<3>(i*3) = centroid;
    }
}
