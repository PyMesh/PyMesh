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

TEST_F(VoxelCircumCenterAttributeTest, irregular_tet) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.5, 1.0, 0.0,
                0.5, 0.0, -1.0;
    MatrixIr faces(0, 3);
    MatrixIr voxels(1, 4);
    voxels << 0,1,2,3;
    MeshPtr mesh = load_data(vertices, faces, voxels);

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
