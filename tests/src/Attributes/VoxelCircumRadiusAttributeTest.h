/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelCircumRadiusAttributeTest : public TestBase {
};

TEST_F(VoxelCircumRadiusAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    mesh->add_attribute("voxel_circumcenter");
    ASSERT_TRUE(mesh->has_attribute("voxel_circumcenter"));
    mesh->add_attribute("voxel_circumradius");
    VectorF circumradius = mesh->get_attribute("voxel_circumradius");
    VectorF circumcenter = mesh->get_attribute("voxel_circumcenter");
    ASSERT_EQ(3, circumcenter.size());
    ASSERT_EQ(1, circumradius.size());

    Float dist_0 = (circumcenter - mesh->get_vertex(0)).norm();
    Float dist_1 = (circumcenter - mesh->get_vertex(1)).norm();
    Float dist_2 = (circumcenter - mesh->get_vertex(2)).norm();
    Float dist_3 = (circumcenter - mesh->get_vertex(3)).norm();

    ASSERT_FLOAT_EQ(circumradius[0], dist_1);
    ASSERT_FLOAT_EQ(circumradius[0], dist_2);
    ASSERT_FLOAT_EQ(circumradius[0], dist_3);
    ASSERT_FLOAT_EQ(circumradius[0], dist_0);
}
