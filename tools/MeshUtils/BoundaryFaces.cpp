/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "BoundaryFaces.h"
#include <algorithm>
#include <cassert>
#include <vector>
#include <set>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Misc/TripletMap.h>
#include <Mesh.h>

using namespace PyMesh;

BoundaryFaces::BoundaryFaces(const Mesh& mesh) {
    extract_boundary(mesh);
    extract_boundary_nodes();
}

size_t BoundaryFaces::get_num_boundaries() const {
    return m_boundaries.rows();
}

MatrixIr BoundaryFaces::get_boundaries() const {
    return m_boundaries;
}

VectorI BoundaryFaces::get_boundary(size_t bi) const {
    assert(bi < m_boundaries.rows());
    return m_boundaries.row(bi);
}

size_t BoundaryFaces::get_boundary_element(size_t bi) const {
    assert(bi < m_boundary_voxels.size());
    return m_boundary_voxels[bi];
}

size_t BoundaryFaces::get_num_boundary_nodes() const {
    return m_boundary_nodes.size();
}

VectorI BoundaryFaces::get_boundary_nodes() const {
    return m_boundary_nodes;
}

void BoundaryFaces::extract_boundary(const Mesh& mesh) {
    typedef TripletMap<size_t> FaceVoxelMap;
    FaceVoxelMap face_voxel_map;

    const size_t num_vertex_per_voxel = mesh.get_vertex_per_voxel();
    const size_t num_voxels = mesh.get_num_voxels();

    if (num_voxels == 0) {
        throw RuntimeError("Mesh has zero voxels!");
    }
    if (num_vertex_per_voxel != 4) {
        throw NotImplementedError("Only tet mesh is supported");
    }

    for (size_t i=0; i<num_voxels; i++) {
        VectorI voxel = mesh.get_voxel(i);
        Triplet voxel_faces[4] = {
            Triplet(voxel[0], voxel[2], voxel[1]),
            Triplet(voxel[0], voxel[1], voxel[3]),
            Triplet(voxel[0], voxel[3], voxel[2]),
            Triplet(voxel[1], voxel[2], voxel[3])
        };
        face_voxel_map.insert(voxel_faces[0], i);
        face_voxel_map.insert(voxel_faces[1], i);
        face_voxel_map.insert(voxel_faces[2], i);
        face_voxel_map.insert(voxel_faces[3], i);
    }

    std::vector<size_t> boundaries;
    std::vector<size_t> boundary_voxels;
    for (FaceVoxelMap::const_iterator itr = face_voxel_map.begin();
            itr != face_voxel_map.end(); itr++) {
        if (itr->second.size() == 1) {
            boundaries.insert(boundaries.end(),
                    itr->first.get_ori_data().data(),
                    itr->first.get_ori_data().data()+3);
            boundary_voxels.push_back(itr->second[0]);
        }
    }

    m_boundaries.resize(boundaries.size() / 3, 3);
    std::copy(boundaries.begin(), boundaries.end(), m_boundaries.data());
    m_boundary_voxels.resize(boundary_voxels.size());
    std::copy(boundary_voxels.begin(), boundary_voxels.end(),
            m_boundary_voxels.data());
}

void BoundaryFaces::extract_boundary_nodes() {
    size_t num_entries = m_boundaries.rows() * m_boundaries.cols();
    std::set<size_t> vertex_set(m_boundaries.data(),
            m_boundaries.data() + num_entries);
    m_boundary_nodes.resize(vertex_set.size());
    std::copy(vertex_set.begin(), vertex_set.end(),
            m_boundary_nodes.data());
}
