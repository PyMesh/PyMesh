/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <Core/Exception.h>

class VoxelFaceIndexAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("voxel_face_index");
            return r;
        }

        void validate_voxel_face_indices(MeshPtr mesh) {
            ASSERT_TRUE(mesh->has_attribute("voxel_face_index"));
            mesh->enable_connectivity();
            const auto& voxel_face_indices =
                mesh->get_attribute("voxel_face_index");
            const auto& faces = mesh->get_faces();
            const auto& voxels = mesh->get_voxels();

            const size_t num_voxels = mesh->get_num_voxels();
            for (size_t i=0; i<num_voxels; i++) {
                Vector4I voxel = voxels.segment<4>(i*4);
                for (size_t j=0; j<4; j++) {
                    const auto fid = voxel_face_indices[i*4+j];
                    if (fid >= 0) {
                        const Vector3I f = faces.segment<3>(fid*3);
                        const auto& adj_voxels =
                            mesh->get_face_adjacent_voxels(fid);
                        ASSERT_EQ(1, adj_voxels.size());
                        ASSERT_EQ(i, adj_voxels[0]);
                        ASSERT_TRUE(voxel[j] != f[0] &&
                                    voxel[j] != f[1] &&
                                    voxel[j] != f[2]);
                    }
                }
            }
        }
};

TEST_F(VoxelFaceIndexAttributeTest, cube) {
    MeshPtr mesh = load_mesh("cube.msh");
    validate_voxel_face_indices(mesh);
}

TEST_F(VoxelFaceIndexAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    validate_voxel_face_indices(mesh);
}

TEST_F(VoxelFaceIndexAttributeTest, hex) {
    ASSERT_THROW(load_mesh("hex.msh"), NotImplementedError);
}
