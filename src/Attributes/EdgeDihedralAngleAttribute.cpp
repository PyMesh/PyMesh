/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeDihedralAngleAttribute.h"

#include <cmath>
#include <iostream>

#include <Mesh.h>

#include <Core/Exception.h>
#include <Math/MatrixUtils.h>
#include <Misc/Triplet.h>
#include <Misc/TripletMap.h>

using namespace PyMesh;

namespace EdgeDihedralAngleAttributeHelper {
    Float angle(const Eigen::Ref<Vector3F>& n1,
            const Eigen::Ref<Vector3F>& n2) {
        return atan2(n1.cross(n2).norm(), n1.dot(n2));
    }
}
using namespace EdgeDihedralAngleAttributeHelper;

void EdgeDihedralAngleAttribute::compute_from_mesh(Mesh& mesh) {
    typedef TripletMap<size_t> EdgeMap;
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (dim != 3) {
        throw NotImplementedError(
                "Dihedral angle computation supports 3D mesh only.");
    }

    if (!mesh.has_attribute("face_normal"))
        mesh.add_attribute("face_normal");
    MatrixFr face_normals =
        MatrixUtils::reshape<MatrixFr>(mesh.get_attribute("face_normal"),
                num_faces, dim);
    if (!face_normals.allFinite()) {
        std::cerr << "Warning: Face normal contains nan or inf!" << std::endl;
        std::cerr << "         Dihedral angle computation would be unreliable."
            << std::endl;
    }

    EdgeMap edge_map;
    EdgeMap edge_index;
    for (size_t i=0; i<num_faces; i++) {
        auto f = mesh.get_face(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            Triplet key(f[j], f[(j+1)%vertex_per_face]);
            edge_map.insert(key, i);
            edge_index.insert(key, i*vertex_per_face + j);
        }
    }

    VectorF& dihedral_angles = m_values;
    dihedral_angles = VectorF::Zero(num_faces * vertex_per_face);
    for (const auto& entry : edge_map) {
        const auto& key = entry.first;
        const auto& val = entry.second;
        size_t num_adj_faces = val.size();
        assert(num_adj_faces > 0);
        Float normal_angle = 0.0;
        if (num_adj_faces == 1) {
            normal_angle = 0.0;
        } else if (num_adj_faces == 2) {
            normal_angle = angle(
                    face_normals.row(val[0]), face_normals.row(val[1]));
        } else {
            for (size_t i=0; i<num_adj_faces; i++) {
                for (size_t j=i+1; j<num_adj_faces; j++) {
                    normal_angle = std::max(normal_angle,
                            angle(face_normals.row(val[0]),
                                face_normals.row(val[1])));
                }
            }
        }

        const auto& edge_indices = edge_index[key];
        for (auto index: edge_indices) {
            dihedral_angles[index] = normal_angle;
        }
    }
}
