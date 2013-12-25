#include "BoundaryFaces.h"
#include <algorithm>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Mesh.h>

#include "TripletMap.h"

BoundaryFaces::BoundaryFaces(const Mesh& mesh) {
    extract_boundary(mesh);
}

size_t BoundaryFaces::get_num_boundaries() const {
    return m_boundaries.rows();
}

MatrixIr BoundaryFaces::get_boundaries() const {
    return m_boundaries;
}

VectorI BoundaryFaces::get_boundary(size_t bi) const {
    return m_boundaries.row(bi);
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
    for (FaceVoxelMap::const_iterator itr = face_voxel_map.begin();
            itr != face_voxel_map.end(); itr++) {
        if (itr->second.size() == 1) {
            boundaries.insert(boundaries.end(),
                    itr->first.get_ori_data().data(),
                    itr->first.get_ori_data().data()+3);
        }
    }

    m_boundaries.resize(boundaries.size() / 3, 3);
    std::copy(boundaries.begin(), boundaries.end(), m_boundaries.data());
}
