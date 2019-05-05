/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <MeshUtils/VoxelUtils.h>
#include <TestBase.h>

TEST(VoxelUtilsTest, simple) {
    using namespace PyMesh;
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0;
    MatrixIr tets(3, 4);
    tets << 0, 1, 2, 3, // Positive
            0, 0, 1, 2, // Degenerate
            0, 1, 3, 2; // Inverted

    auto orientations = VoxelUtils::get_tet_orientations(vertices, tets);
    ASSERT_EQ(3, orientations.size());
    ASSERT_LT(0, orientations[0]);
    ASSERT_EQ(0, orientations[1]);
    ASSERT_GT(0, orientations[2]);
}

TEST(VoxelUtilsTest, delaunay) {
    using namespace PyMesh;
    MatrixFr vertices(7, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0,
                1.0, 1.0, 1.0,
                1.1, 1.1, 1.1,
                0.9, 0.9, 0.9;

    {
        MatrixIr tets(2, 4);
        tets << 0, 1, 2, 3, // Positive
                1, 2, 3, 4;
        auto result = VoxelUtils::is_delaunay(vertices, tets);
        ASSERT_EQ(0, result[0]);
        ASSERT_EQ(0, result[1]);
    }

    {
        MatrixIr tets(2, 4);
        tets << 0, 1, 2, 3, // Positive
                1, 2, 3, 5;
        auto result = VoxelUtils::is_delaunay(vertices, tets);
        ASSERT_EQ(1, result[0]);
        ASSERT_EQ(1, result[1]);
    }

    {
        MatrixIr tets(2, 4);
        tets << 0, 1, 2, 3, // Positive
                1, 2, 3, 6;
        auto result = VoxelUtils::is_delaunay(vertices, tets);
        ASSERT_EQ(-1, result[0]);
        ASSERT_EQ(-1, result[1]);
    }
}
