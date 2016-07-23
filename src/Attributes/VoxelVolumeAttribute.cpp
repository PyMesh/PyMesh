/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VoxelVolumeAttribute.h"
#include <cassert>
#include <cmath>
#include <iostream>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

using namespace PyMesh;

namespace VoxelVolumeAttributeHelper {
    Float compute_signed_tet_volume(
            const Vector3F& v0, const Vector3F& v1,
            const Vector3F& v2, const Vector3F& v3) {
        // Formula:  volume = <a x b, c> / 6
        //   where a,b and c are edge vector from a single vertex.
        //   <., .> is inner product.
        return ((v1 - v0).cross(v2 - v0)).dot(v3 - v0) / 6.0;
    }

    Float compute_abs_tet_volume(
            const Vector3F& v0, const Vector3F& v1,
            const Vector3F& v2, const Vector3F& v3) {
        return fabs(compute_signed_tet_volume(v0, v1, v2, v3));
    }
}
using namespace VoxelVolumeAttributeHelper;

void VoxelVolumeAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t num_vertex_per_voxel = mesh.get_vertex_per_voxel();

    VectorF& volumes = m_values;
    volumes = VectorF::Zero(num_voxels);

    if (num_voxels > 0) {
        if (num_vertex_per_voxel == 4) {
            for (size_t i=0; i<num_voxels; i++) {
                volumes[i] = compute_signed_tet_volume(mesh, i);
            }
        } else if (num_vertex_per_voxel == 8) {
            for (size_t i=0; i<num_voxels; i++) {
                volumes[i] = compute_signed_hex_volume(mesh, i);
            }
        } else {
            std::cerr << "Unknown voxel type with " << num_vertex_per_voxel
                << " per voxel." << std::endl;
            return;
        }
    }
}

Float VoxelVolumeAttribute::compute_signed_tet_volume(Mesh& mesh, size_t voxel_idx) {
    VectorI voxel = mesh.get_voxel(voxel_idx);
    assert(voxel.size() == 4);

    Vector3F v[4] = {
        mesh.get_vertex(voxel[0]),
        mesh.get_vertex(voxel[1]),
        mesh.get_vertex(voxel[2]),
        mesh.get_vertex(voxel[3])
    };

    return ::compute_signed_tet_volume(v[0], v[1], v[2], v[3]);
}

Float VoxelVolumeAttribute::compute_signed_hex_volume(Mesh& mesh, size_t voxel_idx) {
    VectorI voxel = mesh.get_voxel(voxel_idx);
    assert(voxel.size() == 8);
    //             v
    //      3----------2
    //      |\     ^   |\
    //      | \    |   | \
    //      |  \   |   |  \
    //      |   7------+---6
    //      |   |  +-- |-- | -> u
    //      0---+---\--1   |
    //       \  |    \  \  |
    //        \ |     \  \ |
    //         \|      w  \|
    //          4----------5

    Vector3F v[8] = {
        mesh.get_vertex(voxel[0]),
        mesh.get_vertex(voxel[1]),
        mesh.get_vertex(voxel[2]),
        mesh.get_vertex(voxel[3]),
        mesh.get_vertex(voxel[4]),
        mesh.get_vertex(voxel[5]),
        mesh.get_vertex(voxel[6]),
        mesh.get_vertex(voxel[7])
    };

    Vector3F face_centers[6] = {
        0.25 * (v[0] + v[1] + v[2] + v[3]),
        0.25 * (v[4] + v[5] + v[6] + v[7]),
        0.25 * (v[0] + v[1] + v[5] + v[4]),
        0.25 * (v[2] + v[3] + v[7] + v[6]),
        0.25 * (v[0] + v[4] + v[7] + v[3]),
        0.25 * (v[1] + v[2] + v[6] + v[5]),
    };

    Vector3F centroid = 0.125 * (
            v[0] + v[1] + v[2] + v[3] +
            v[4] + v[5] + v[6] + v[7]);

    return 
        ::compute_abs_tet_volume(centroid, face_centers[0], v[0], v[1]) +
        ::compute_abs_tet_volume(centroid, face_centers[0], v[1], v[2]) +
        ::compute_abs_tet_volume(centroid, face_centers[0], v[2], v[3]) +
        ::compute_abs_tet_volume(centroid, face_centers[0], v[3], v[0]) +

        ::compute_abs_tet_volume(centroid, face_centers[1], v[4], v[5]) +
        ::compute_abs_tet_volume(centroid, face_centers[1], v[5], v[6]) +
        ::compute_abs_tet_volume(centroid, face_centers[1], v[6], v[7]) +
        ::compute_abs_tet_volume(centroid, face_centers[1], v[7], v[4]) +

        ::compute_abs_tet_volume(centroid, face_centers[2], v[0], v[1]) +
        ::compute_abs_tet_volume(centroid, face_centers[2], v[1], v[5]) +
        ::compute_abs_tet_volume(centroid, face_centers[2], v[5], v[4]) +
        ::compute_abs_tet_volume(centroid, face_centers[2], v[4], v[0]) +

        ::compute_abs_tet_volume(centroid, face_centers[3], v[2], v[3]) +
        ::compute_abs_tet_volume(centroid, face_centers[3], v[3], v[7]) +
        ::compute_abs_tet_volume(centroid, face_centers[3], v[7], v[6]) +
        ::compute_abs_tet_volume(centroid, face_centers[3], v[6], v[2]) +

        ::compute_abs_tet_volume(centroid, face_centers[4], v[0], v[4]) +
        ::compute_abs_tet_volume(centroid, face_centers[4], v[4], v[7]) +
        ::compute_abs_tet_volume(centroid, face_centers[4], v[7], v[3]) +
        ::compute_abs_tet_volume(centroid, face_centers[4], v[3], v[0]) +

        ::compute_abs_tet_volume(centroid, face_centers[5], v[1], v[2]) +
        ::compute_abs_tet_volume(centroid, face_centers[5], v[2], v[6]) +
        ::compute_abs_tet_volume(centroid, face_centers[5], v[6], v[5]) +
        ::compute_abs_tet_volume(centroid, face_centers[5], v[5], v[1]) ;
}
