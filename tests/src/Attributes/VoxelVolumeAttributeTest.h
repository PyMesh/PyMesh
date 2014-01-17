#pragma once

#include <string>

#include <Misc/Environment.h>

class VoxelVolumeAttributeTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string proj_root =
                Environment::get_required("PYMESH_PATH");
            m_data_dir = proj_root + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .with_attribute("voxel_volume")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VoxelVolumeAttributeTest, Positivity) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    ASSERT_TRUE(cube_tet->has_attribute("voxel_volume"));
    VectorF cube_voxel_volumes = cube_tet->get_attribute("voxel_volume");
    ASSERT_EQ(cube_tet->get_num_voxels(), cube_voxel_volumes.size());
    ASSERT_GT(cube_voxel_volumes.size(), 0.0);
    ASSERT_GT(cube_voxel_volumes.minCoeff(), 0.0);
}

TEST_F(VoxelVolumeAttributeTest, Recomputation) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    VectorF cube_tet_voxel_volume_1 = cube_tet->get_attribute("voxel_volume");

    // Recompute voxel volume
    cube_tet->remove_attribute("voxel_volume");
    cube_tet->add_attribute("voxel_volume");
    VectorF cube_tet_voxel_volume_2 = cube_tet->get_attribute("voxel_volume");
    ASSERT_EQ(cube_tet_voxel_volume_1, cube_tet_voxel_volume_2);
}

TEST_F(VoxelVolumeAttributeTest, ZeroVoxel) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_TRUE(square->has_attribute("voxel_volume"));
    ASSERT_EQ(0, square->get_attribute("voxel_volume").size());
}
