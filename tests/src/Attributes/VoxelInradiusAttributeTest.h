/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelInradiusAttributeTest : public TestBase {
};

TEST_F(VoxelInradiusAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    mesh->add_attribute("voxel_inradius");
    ASSERT_TRUE(mesh->has_attribute("voxel_inradius"));
    VectorF inradius = mesh->get_attribute("voxel_inradius");
    ASSERT_EQ(1, inradius.size());

    mesh->add_attribute("voxel_volume");
    VectorF volume = mesh->get_attribute("voxel_volume");
    ASSERT_EQ(1, volume.size());

    mesh->add_attribute("face_area");
    VectorF area = mesh->get_attribute("face_area");
    ASSERT_EQ(4, area.size());

    ASSERT_FLOAT_EQ(volume[0], area.sum() * inradius[0] / 3.0);
}
