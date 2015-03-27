/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelVolumeAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("voxel_volume");
            return r;
        }
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

TEST_F(VoxelVolumeAttributeTest, hex) {
    MeshPtr hex = load_mesh("hex.msh");
    ASSERT_TRUE(hex->has_attribute("voxel_volume"));
    VectorF volumes = hex->get_attribute("voxel_volume");
    ASSERT_EQ(1, volumes.size());
    ASSERT_FLOAT_EQ(8.0, volumes[0]);
}
