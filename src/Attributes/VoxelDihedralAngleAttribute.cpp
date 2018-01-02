/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "VoxelDihedralAngleAttribute.h"

#include <cmath>
#include <Eigen/Core>

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

namespace VoxelDihedralAngleAttributeHelper {
    Float angle(const Eigen::Ref<Vector3F>& n1,
            const Eigen::Ref<Vector3F>& n2) {
        return atan2(n1.cross(n2).norm(), n1.dot(n2));
    }

    Vector3F compute_normal(
            const Vector3F& v1,
            const Vector3F& v2,
            const Vector3F& v3) {
        return (v2 - v1).cross(v3 - v1);
    }
}

using namespace VoxelDihedralAngleAttributeHelper;

void VoxelDihedralAngleAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();
    if (dim != 3) {
        throw RuntimeError("Voxel dihedral anlge computation is for 3D only.");
    }
    if (num_voxels > 0 && vertex_per_voxel != 4) {
        throw NotImplementedError(
                "Voxel dihedral angle computation only support tet for now.");
    }

    const auto& vertices = mesh.get_vertices();
    const auto& voxels = mesh.get_voxels();
    VectorF& dihedral_angles = m_values;
    dihedral_angles.resize(num_voxels * 6);

    for (size_t i=0; i<num_voxels; i++) {
        Vector4I v = voxels.segment<4>(i*4);
        Vector3F v0 = vertices.segment<3>(v[0]*3);
        Vector3F v1 = vertices.segment<3>(v[1]*3);
        Vector3F v2 = vertices.segment<3>(v[2]*3);
        Vector3F v3 = vertices.segment<3>(v[3]*3);

        Vector3F n0 = compute_normal(v1, v2, v3);
        Vector3F n1 = compute_normal(v0, v3, v2);
        Vector3F n2 = compute_normal(v0, v1, v3);
        Vector3F n3 = compute_normal(v0, v2, v1);

        dihedral_angles[i*6  ] = M_PI - angle(n2, n3);
        dihedral_angles[i*6+1] = M_PI - angle(n0, n3);
        dihedral_angles[i*6+2] = M_PI - angle(n1, n3);
        dihedral_angles[i*6+3] = M_PI - angle(n1, n2);
        dihedral_angles[i*6+4] = M_PI - angle(n0, n1);
        dihedral_angles[i*6+5] = M_PI - angle(n0, n2);
    }
}

