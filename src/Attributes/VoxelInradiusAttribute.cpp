/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "VoxelInradiusAttribute.h"

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void VoxelInradiusAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();

    if (num_voxels > 0 && vertex_per_voxel != 4) {
        throw NotImplementedError(
                "Voxel inradius is defined for 3D tet mesh only.");
    }
    if (dim != 3) {
        throw RuntimeError("Voxel inradius is defined for 3D tet mesh only.");
    }

    VectorF& inradius = m_values;
    inradius.resize(num_voxels);

    const auto& voxels = mesh.get_voxels();
    const auto& vertices = mesh.get_vertices();
    if (!mesh.has_attribute("voxel_volume")) {
        mesh.add_attribute("voxel_volume");
    }
    const auto& volumes = mesh.get_attribute("voxel_volume");

    for (size_t i=0; i<num_voxels; i++) {
        Vector4I voxel = voxels.segment<4>(i*4);
        Vector3F v0 = vertices.segment<3>(voxel[0]*3);
        Vector3F v1 = vertices.segment<3>(voxel[1]*3);
        Vector3F v2 = vertices.segment<3>(voxel[2]*3);
        Vector3F v3 = vertices.segment<3>(voxel[3]*3);

        Float a012 = ((v1-v0).cross(v2-v0)).norm();
        Float a023 = ((v2-v0).cross(v3-v0)).norm();
        Float a013 = ((v1-v0).cross(v3-v0)).norm();
        Float a123 = ((v1-v3).cross(v2-v3)).norm();
        Float sum = (a012 + a023 + a013 + a123) * 0.5;

        inradius[i] = volumes[i] / sum * 3.0;
    }
}
