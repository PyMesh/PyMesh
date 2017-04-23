/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelEdgeRatioAttributeTest : public TestBase {
};

TEST_F(VoxelEdgeRatioAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    mesh->add_attribute("voxel_edge_ratio");
    ASSERT_TRUE(mesh->has_attribute("voxel_edge_ratio"));
    VectorF edge_ratios = mesh->get_attribute("voxel_edge_ratio");
    ASSERT_EQ(1, edge_ratios.size());
    ASSERT_NEAR(1.0, edge_ratios[0], 1e-6);
}
