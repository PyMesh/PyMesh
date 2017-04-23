/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelCircumCenterAttributeTest : public TestBase {
};

TEST_F(VoxelCircumCenterAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    mesh->add_attribute("voxel_circumcenter");
    ASSERT_TRUE(mesh->has_attribute("voxel_circumcenter"));
    VectorF circumcenter = mesh->get_attribute("voxel_circumcenter");
    ASSERT_EQ(3, circumcenter.size());

    Float dist_0 = (circumcenter - mesh->get_vertex(0)).norm();
    Float dist_1 = (circumcenter - mesh->get_vertex(1)).norm();
    Float dist_2 = (circumcenter - mesh->get_vertex(2)).norm();
    Float dist_3 = (circumcenter - mesh->get_vertex(3)).norm();

    ASSERT_FLOAT_EQ(dist_0, dist_1);
    ASSERT_FLOAT_EQ(dist_1, dist_2);
    ASSERT_FLOAT_EQ(dist_2, dist_3);
    ASSERT_FLOAT_EQ(dist_3, dist_0);
}
