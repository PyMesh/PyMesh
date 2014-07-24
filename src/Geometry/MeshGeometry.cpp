#include "MeshGeometry.h"

#include <map>
#include <sstream>
#include <vector>

#include <Core/Exception.h>
#include <Misc/Triplet.h>

void MeshGeometry::extract_faces_from_voxels() {
    if (m_vertex_per_voxel != 4) {
        throw NotImplementedError("Only tet voxel is supported.");
    }

    const VectorI& voxels = m_voxels;
    typedef std::map<Triplet, int> FaceCounter;
    FaceCounter face_counter;

    for (size_t i=0; i<voxels.size(); i+= m_vertex_per_voxel) {
        VectorI voxel = voxels.segment(i, m_vertex_per_voxel);
        // TODO: only tet mesh is handled.
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
            // TODO: only triangles is handled.
            assert(f.size() == 3);
            vertex_buffer.push_back(f[0]);
            vertex_buffer.push_back(f[1]);
            vertex_buffer.push_back(f[2]);
        }
    }

    m_faces.resize(vertex_buffer.size());
    std::copy(vertex_buffer.begin(), vertex_buffer.end(), m_faces.data());
}
