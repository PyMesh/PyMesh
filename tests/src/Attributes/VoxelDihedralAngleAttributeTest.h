/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelDihedralAngleAttributeTest : public TestBase {
};

TEST_F(VoxelDihedralAngleAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    mesh->add_attribute("voxel_dihedral_angle");
    ASSERT_TRUE(mesh->has_attribute("voxel_dihedral_angle"));
    VectorF dihedral_angles = mesh->get_attribute("voxel_dihedral_angle");
    ASSERT_EQ(6, dihedral_angles.size());
    ASSERT_FLOAT_EQ(acos(1.0/3.0), dihedral_angles[0]);
    ASSERT_FLOAT_EQ(acos(1.0/3.0), dihedral_angles[1]);
    ASSERT_FLOAT_EQ(acos(1.0/3.0), dihedral_angles[2]);
    ASSERT_FLOAT_EQ(acos(1.0/3.0), dihedral_angles[3]);
    ASSERT_FLOAT_EQ(acos(1.0/3.0), dihedral_angles[4]);
    ASSERT_FLOAT_EQ(acos(1.0/3.0), dihedral_angles[5]);
}
