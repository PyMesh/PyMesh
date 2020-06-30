/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeDihedralAngleAttribute.h"

#include <array>
#include <cmath>
#include <iostream>
#include <tbb/tbb.h>
#include <mutex>
#include <thread>

#include <Mesh.h>

#include <Core/Exception.h>
#include <Math/MatrixUtils.h>
#include <Misc/Multiplet.h>
#include <Misc/MultipletMap.h>

using namespace PyMesh;

void EdgeDihedralAngleAttribute::compute_from_mesh(Mesh& mesh) {
    using EdgeMap = DupletMap<std::array<size_t, 2>>;
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (dim != 3) {
        throw NotImplementedError(
                "Dihedral angle computation supports 3D mesh only.");
    }

    if (!mesh.has_attribute("face_normal"))
        mesh.add_attribute("face_normal");
    Matrix3Fr face_normals =
        MatrixUtils::reshape<Matrix3Fr>(mesh.get_attribute("face_normal"),
                num_faces, dim);
    if (!face_normals.allFinite()) {
        std::cerr << "Warning: Face normal contains nan or inf!" << std::endl;
        std::cerr << "         Dihedral angle computation would be unreliable."
            << std::endl;
    }

    EdgeMap edge_map;
    for (size_t i=0; i<num_faces; i++) {
        const auto f = mesh.get_face(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            const Duplet key(f[j], f[(j+1)%vertex_per_face]);
            edge_map.insert(key, {i, j});
        }
    }

    auto angle = [&face_normals](size_t f1, size_t f2) {
        const Eigen::Ref<Vector3F>& n1 = face_normals.row(f1).transpose();
        const Eigen::Ref<Vector3F>& n2 = face_normals.row(f2).transpose();
        return atan2(n1.cross(n2).norm(), n1.dot(n2));
    };

    VectorF& dihedral_angles = m_values;
    dihedral_angles = VectorF::Zero(num_faces * vertex_per_face);

    auto compute_kernel = [&angle, &dihedral_angles, vertex_per_face](
            EdgeMap::const_range_type& r) {
        for (const auto& entry : r) {
            const auto& key = entry.first;
            const auto& val = entry.second;
            const size_t num_adj_faces = val.size();
            assert(num_adj_faces > 0);
            Float normal_angle = 0.0;
            if (num_adj_faces == 1) {
                normal_angle = 0.0;
            } else if (num_adj_faces == 2) {
                normal_angle = angle(val[0][0], val[1][0]);
            } else {
                for (size_t i=0; i<num_adj_faces; i++) {
                    for (size_t j=i+1; j<num_adj_faces; j++) {
                        normal_angle = std::max(normal_angle,
                                angle(val[i][0], val[j][0]));
                    }
                }
            }

            for (const auto& itr : val) {
                dihedral_angles[itr[0] * vertex_per_face + itr[1]] = normal_angle;
            }
        }
    };
    tbb::parallel_for(edge_map.range(), compute_kernel);
}
