#include "VoxelVolumeAttribute.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include "Mesh.h"

void VoxelVolumeAttribute::compute_from_mesh(Mesh& mesh) {
    size_t num_voxels = mesh.get_num_voxels();
    size_t num_vertex_per_voxel = mesh.get_vertex_per_voxel();

    VectorF& volumes = m_values;
    volumes = VectorF::Zero(num_voxels);

    if (num_vertex_per_voxel == 4) {
        for (size_t i=0; i<num_voxels; i++) {
            volumes[i] = compute_tet_volume(mesh, i);
        }
    } else {
        std::cerr << "Unknown voxel type with " << num_vertex_per_voxel
            << " per voxel." << std::endl;
        return;
    }
}

Float VoxelVolumeAttribute::compute_tet_volume(Mesh& mesh, size_t voxel_idx) {
    VectorI voxel = mesh.get_voxel(voxel_idx);
    assert(voxel.size() == 4);

    Vector3F v[4] = {
        mesh.get_vertex(voxel[0]),
        mesh.get_vertex(voxel[1]),
        mesh.get_vertex(voxel[2]),
        mesh.get_vertex(voxel[3])
    };

    // Formula:  volume = <a x b, c> / 6
    //   where a,b and c are edge vector from a single vertex.
    //   <., .> is inner product.
    Float volume = ((v[1] - v[0]).cross(v[2] - v[0])).dot(v[3] - v[0]) / 6.0;
    return fabs(volume);
}
