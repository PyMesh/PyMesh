/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "VoxelIncenterAttribute.h"

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void VoxelIncenterAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();

    if (num_voxels > 0 && vertex_per_voxel != 4) {
        throw NotImplementedError(
                "Voxel incenter is defined for 3D tet mesh only.");
    }
    if (dim != 3) {
        throw RuntimeError("Voxel incenter is defined for 3D tet mesh only.");
    }

    VectorF& incenters = m_values;
    incenters.resize(num_voxels * 3);
    const auto& voxels = mesh.get_voxels();
    const auto& vertices = mesh.get_vertices();

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
        Float sum = a012 + a023 + a013 + a123;

        incenters.segment<3>(i*3) =
            a123/sum * v0 + a023/sum * v1 +
            a013/sum * v2 + a012/sum * v3;
    }
}
