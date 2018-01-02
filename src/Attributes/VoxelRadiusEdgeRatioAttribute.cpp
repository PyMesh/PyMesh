/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */

#include "VoxelRadiusEdgeRatioAttribute.h"

#include <limits>
#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

void VoxelRadiusEdgeRatioAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();
    if (dim != 3) {
        throw RuntimeError("Voxel radius/edge ratio computation is for 3D only.");
    }
    if (num_voxels > 0 && vertex_per_voxel != 4) {
        throw NotImplementedError(
                "Voxel radius/edge ratio computation only support tet for now.");
    }

    if (!mesh.has_attribute("voxel_circumradius")) {
        mesh.add_attribute("voxel_circumradius");
    }
    const auto& circum_radii = mesh.get_attribute("voxel_circumradius");
    assert(circum_radii.size() == num_voxels);
    const auto& vertices = mesh.get_vertices();
    const auto& voxels = mesh.get_voxels();
    VectorF& re_ratio = m_values;
    re_ratio.resize(num_voxels);

    for (size_t i=0; i<num_voxels; i++) {
        Vector4I v = voxels.segment<4>(i*4);
        Vector3F v0 = vertices.segment<3>(v[0]*3);
        Vector3F v1 = vertices.segment<3>(v[1]*3);
        Vector3F v2 = vertices.segment<3>(v[2]*3);
        Vector3F v3 = vertices.segment<3>(v[3]*3);

        Eigen::Matrix<Float, 6, 1> edge_lengths;
        edge_lengths[0] = (v0 -v1).norm();
        edge_lengths[1] = (v0 -v2).norm();
        edge_lengths[2] = (v0 -v3).norm();
        edge_lengths[3] = (v1 -v2).norm();
        edge_lengths[4] = (v2 -v3).norm();
        edge_lengths[5] = (v1 -v3).norm();

        Float min_edge = edge_lengths.minCoeff();
        if (min_edge == 0.0) {
            re_ratio[i] = std::numeric_limits<Float>::infinity();
        } else {
            re_ratio[i] = circum_radii[i] / min_edge;
        }
    }
}
