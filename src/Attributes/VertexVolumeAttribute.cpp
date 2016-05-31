/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexVolumeAttribute.h"

#include <string>

#include <Mesh.h>

using namespace PyMesh;

void VertexVolumeAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t num_vertex_per_voxel = mesh.get_vertex_per_voxel();
    if (dim != 3 || num_voxels == 0) return;

    VectorI& voxels = mesh.get_voxels();
    VectorF& volumes = get_voxel_volumes(mesh);
    VectorF& vertex_volumes = m_values;

    vertex_volumes = VectorF::Zero(num_vertices);

    for (size_t i=0; i<num_voxels; i++) {
        for (size_t j=0; j<num_vertex_per_voxel; j++) {
            size_t index = i*num_vertex_per_voxel + j;
            vertex_volumes[voxels[index]] += volumes[i];
        }
    }

    vertex_volumes /= num_vertex_per_voxel;
}

VectorF& VertexVolumeAttribute::get_voxel_volumes(Mesh& mesh) {
    std::string voxel_volume_attribute("voxel_volume");
    if (!mesh.has_attribute(voxel_volume_attribute)) {
        mesh.add_attribute(voxel_volume_attribute);
    }
    return mesh.get_attribute(voxel_volume_attribute);
}
