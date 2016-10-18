/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <Triangulation.h>
#include <MeshUtils/MeshChecker.h>

class TriangulationTest : public TestBase {
    public:
        void assert_valid_triangulation(
                const MatrixFr& vertices, const MatrixIr& faces) {
            MatrixIr voxels(0, 4);
            MeshChecker checker(vertices, faces, voxels);
            ASSERT_TRUE(checker.is_oriented());
            ASSERT_TRUE(checker.is_edge_manifold());
            ASSERT_TRUE(checker.is_vertex_manifold());
            ASSERT_EQ(1, checker.get_num_connected_components());
        }
};
