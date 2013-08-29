#include "VoxelCentroidAttribute.h"

#include "Mesh.h"

void VoxelCentroidAttribute::compute_from_mesh(Mesh& mesh) {
    size_t num_voxels = mesh.get_num_voxels();

    VectorF& centroids = m_values;
    centroids.resize(num_voxels*3);

    for (size_t i=0; i<num_voxels; i++) {
        VectorI voxel = mesh.get_voxel(i);

        Vector3F centroid = Vector3F::Zero();
        for (size_t j=0; j<voxel.size(); j++) {
            centroid += mesh.get_vertex(voxel[j]);
        }
        centroid /= voxel.size();

        centroids.segment<3>(i*3) = centroid;
    }
}
