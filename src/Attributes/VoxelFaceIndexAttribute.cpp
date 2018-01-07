/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#include "VoxelFaceIndexAttribute.h"

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void VoxelFaceIndexAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();
    if (vertex_per_voxel != 4) {
        throw NotImplementedError("Only tet mesh is supported for voxel_face_index");
    }

    mesh.enable_voxel_connectivity();

    auto& indices = m_values;
    indices.resize(num_voxels * 4);
    indices.setConstant(-1.0);

    const auto& faces = mesh.get_faces();
    const auto& voxels = mesh.get_voxels();
    for (size_t i=0; i<num_voxels; i++) {
        const Vector4I voxel = voxels.segment<4>(i*4);
        const auto& adj_faces = mesh.get_voxel_adjacent_faces(i);
        const size_t num_adj_faces = adj_faces.size();
        for (size_t j=0; j<num_adj_faces; j++) {
            const Vector3I f = faces.segment<3>(adj_faces[j]*3);
            if (voxel[0] != f[0] &&
                voxel[0] != f[1] &&
                voxel[0] != f[2]) {
                indices[i*4] =  adj_faces[j];
            } else if (voxel[1] != f[0] &&
                       voxel[1] != f[1] &&
                       voxel[1] != f[2]) {
                indices[i*4+1] =  adj_faces[j];
            } else if (voxel[2] != f[0] &&
                       voxel[2] != f[1] &&
                       voxel[2] != f[2]) {
                indices[i*4+2] =  adj_faces[j];
            } else if (voxel[3] != f[0] &&
                       voxel[3] != f[1] &&
                       voxel[3] != f[2]) {
                indices[i*4+3] =  adj_faces[j];
            }
        }
    }
}
