/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <MeshUtils/EdgeUtils.h>
#include <TestBase.h>

class EdgeUtilsTest : public TestBase {
};

TEST_F(EdgeUtilsTest, edge_loop) {
    MatrixIr edges(3, 2);
    edges << 1, 2,
             2, 3,
             3, 1;
    auto chains = EdgeUtils::chain_edges(edges);
    ASSERT_EQ(1, chains.size());
    ASSERT_EQ(6, chains.front().sum());
}

TEST_F(EdgeUtilsTest, multiple_loops) {
    MatrixIr edges(6, 2);
    edges << 1, 2,
             2, 3,
             3, 1,
             0, 4,
             4, 6,
             6, 0;
    auto chains = EdgeUtils::chain_edges(edges);
    ASSERT_EQ(2, chains.size());
}

TEST_F(EdgeUtilsTest, open_loop) {
    MatrixIr edges(3, 2);
    edges << 1, 2,
             2, 3,
             3, 4;
    auto chains = EdgeUtils::chain_edges(edges);
    ASSERT_EQ(1, chains.size());
    ASSERT_EQ(10, chains.front().sum());
}

TEST_F(EdgeUtilsTest, multiple_open_loops) {
    MatrixIr edges(6, 2);
    edges << 1, 2,
             2, 3,
             3, 4,
             6, 7,
             7, 8,
             8, 9;
    auto chains = EdgeUtils::chain_edges(edges);
    ASSERT_EQ(2, chains.size());
}

TEST_F(EdgeUtilsTest, non_simple_chains) {
    MatrixIr edges(7, 2);
    edges << 0, 1,
             1, 2,
             2, 3,
             3, 4,
             4, 5,
             5, 0,
             5, 2;
    ASSERT_THROW(EdgeUtils::chain_edges(edges), RuntimeError);
}

