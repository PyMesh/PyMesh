/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshGeometry.h"

#include <map>
#include <sstream>
#include <vector>
#include <iostream>

#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Misc/Multiplet.h>

using namespace PyMesh;

void MeshGeometry::extract_faces_from_voxels() {
    if (m_vertex_per_voxel == 4) {
        extract_faces_from_tets();
    } else if (m_vertex_per_voxel == 8) {
        extract_faces_from_hexes();
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported voxel type with " << m_vertex_per_voxel << " vertices per voxel";
        throw NotImplementedError(err_msg.str());
    }
}

int MeshGeometry::project_out_zero_dim() {
    if (m_dim == 2) return -1;
    assert(m_dim == 3);
    const size_t num_vertices = get_num_vertices();
    MatrixFr vertices(num_vertices, m_dim);
    std::copy(m_vertices.data(), m_vertices.data() + m_vertices.size(),
            vertices.data());
    Vector3F max_coord = vertices.colwise().maxCoeff();
    Vector3F min_coord = vertices.colwise().minCoeff();

    int zero_dim = -1;
    for (size_t i=0; i<3; i++) {
        if (max_coord[i] == min_coord[i]) {
            zero_dim = i;
            break;
        }
    }

    if (zero_dim == -1) return -1;
    m_dim = 2;
    m_vertices.resize(num_vertices * 2);
    if (zero_dim == 0) {
        // Drop X
        //std::cout << "Removing X component" << std::endl;
        for (size_t i=0; i<num_vertices; i++) {
            m_vertices[i*2  ] = vertices.row(i)[1];
            m_vertices[i*2+1] = vertices.row(i)[2];
        }
    } else if (zero_dim == 2) {
        // Drop Z
        //std::cout << "Removing Z component" << std::endl;
        for (size_t i=0; i<num_vertices; i++) {
            m_vertices[i*2  ] = vertices.row(i)[0];
            m_vertices[i*2+1] = vertices.row(i)[1];
        }
    } else {
        // Drop Y
        //std::cout << "Removing Y component" << std::endl;
        for (size_t i=0; i<num_vertices; i++) {
            m_vertices[i*2  ] = vertices.row(i)[0];
            m_vertices[i*2+1] = vertices.row(i)[2];
        }
    }
    return zero_dim;
}

void MeshGeometry::extract_faces_from_tets() {
    const VectorI& voxels = m_voxels;
    typedef std::map<Triplet, int> FaceCounter;
    FaceCounter face_counter;

    for (size_t i=0; i<voxels.size(); i+= m_vertex_per_voxel) {
        VectorI voxel = voxels.segment(i, m_vertex_per_voxel);
        // Note that the order of vertices below are predefined by MSH format,
        // each face should have normal pointing outward.
        assert(voxel.size() == 4);
        Triplet voxel_faces[4] = {
            Triplet(voxel[0], voxel[2], voxel[1]),
            Triplet(voxel[0], voxel[1], voxel[3]),
            Triplet(voxel[0], voxel[3], voxel[2]),
            Triplet(voxel[1], voxel[2], voxel[3])
        };
        for (size_t j=0; j<4; j++) {
            if (face_counter.find(voxel_faces[j]) == face_counter.end()) {
                face_counter[voxel_faces[j]] = 1;
            } else {
                face_counter[voxel_faces[j]] += 1;
            }
        }
    }

    std::vector<int> vertex_buffer;
    for (FaceCounter::const_iterator itr = face_counter.begin();
            itr!=face_counter.end(); itr++) {
        if (itr->second != 1 && itr->second != 2) {
            const Vector3I& triplet = itr->first.get_ori_data();
            std::stringstream err_msg;
            err_msg << "Non-manifold mesh detected!" << std::endl;
            err_msg << "Face <"
                << triplet[0] << ", "
                << triplet[1] << ", "
                << triplet[2] << "> has "
                << itr->second << " adjacent volume elements";
            throw RuntimeError(err_msg.str());
        }
        if (itr->second == 1) {
            const VectorI& f = itr->first.get_ori_data();
            assert(f.size() == 3);
            vertex_buffer.push_back(f[0]);
            vertex_buffer.push_back(f[1]);
            vertex_buffer.push_back(f[2]);
        }
    }

    m_faces.resize(vertex_buffer.size());
    std::copy(vertex_buffer.begin(), vertex_buffer.end(), m_faces.data());
    m_vertex_per_face = 3;
}

void MeshGeometry::extract_faces_from_hexes() {
    const VectorI& voxels = m_voxels;
    typedef std::map<Multiplet, int> FaceCounter;
    FaceCounter face_counter;

    for (size_t i=0; i<voxels.size(); i+= m_vertex_per_voxel) {
        VectorI voxel = voxels.segment(i, m_vertex_per_voxel);
        // Note that the order of vertices below are predefined by MSH format,
        // each face should have normal pointing outward.
        assert(voxel.size() == 8);
        Multiplet voxel_faces[6] = {
            Multiplet(Vector4I(voxel[0], voxel[1], voxel[5], voxel[4])), // Bottom
            Multiplet(Vector4I(voxel[2], voxel[3], voxel[7], voxel[6])), // Top
            Multiplet(Vector4I(voxel[0], voxel[4], voxel[7], voxel[3])), // Left
            Multiplet(Vector4I(voxel[1], voxel[2], voxel[6], voxel[5])), // Right
            Multiplet(Vector4I(voxel[4], voxel[5], voxel[6], voxel[7])), // Front
            Multiplet(Vector4I(voxel[0], voxel[3], voxel[2], voxel[1]))  // Back
        };

        for (size_t j=0; j<6; j++) {
            if (face_counter.find(voxel_faces[j]) == face_counter.end()) {
                face_counter[voxel_faces[j]] = 1;
            } else {
                face_counter[voxel_faces[j]] += 1;
            }
        }
    }

    std::vector<int> vertex_buffer;
    for (FaceCounter::const_iterator itr = face_counter.begin();
            itr!=face_counter.end(); itr++) {
        if (itr->second != 1 && itr->second != 2) {
            const Vector4I& face = itr->first.get_ori_data();
            std::stringstream err_msg;
            err_msg << "Non-manifold mesh detected!" << std::endl;
            err_msg << "Face <"
                << face[0] << ", "
                << face[1] << ", "
                << face[2] << ", "
                << face[3] << "> has "
                << itr->second << " adjacent volume elements";
            throw RuntimeError(err_msg.str());
        }
        if (itr->second == 1) {
            const VectorI& f = itr->first.get_ori_data();
            assert(f.size() == 4);
            vertex_buffer.push_back(f[0]);
            vertex_buffer.push_back(f[1]);
            vertex_buffer.push_back(f[2]);
            vertex_buffer.push_back(f[3]);
        }
    }

    m_faces.resize(vertex_buffer.size());
    std::copy(vertex_buffer.begin(), vertex_buffer.end(), m_faces.data());
    m_vertex_per_face = 4;
}
