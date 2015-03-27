/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Misc/BoxChecker.h>

class BoxCheckerTest : public ::testing::Test {
    protected:
        void SetUp() {
            m_bbox_min = Vector3F(-1, -1, -1);
            m_bbox_max = Vector3F( 1,  1,  1);
        }

    protected:
        Vector3F m_bbox_min;
        Vector3F m_bbox_max;
};

TEST_F(BoxCheckerTest, interior_point) {
    BoxChecker checker(m_bbox_min, m_bbox_max);
    Vector3F p(0.0, 0.0, 0.0);
    ASSERT_TRUE(checker.is_inside(p));
    ASSERT_FALSE(checker.is_outside(p));
    ASSERT_FALSE(checker.is_on_boundary(p));
    ASSERT_FALSE(checker.is_on_boundary_edges(p));
    ASSERT_FALSE(checker.is_on_boundary_corners(p));
}

TEST_F(BoxCheckerTest, exterior_point) {
    BoxChecker checker(m_bbox_min, m_bbox_max);
    Vector3F p(0.0, 0.0, 2.0);
    ASSERT_FALSE(checker.is_inside(p));
    ASSERT_TRUE(checker.is_outside(p));
    ASSERT_FALSE(checker.is_on_boundary(p));
    ASSERT_FALSE(checker.is_on_boundary_edges(p));
    ASSERT_FALSE(checker.is_on_boundary_corners(p));
}

TEST_F(BoxCheckerTest, mid_face_point) {
    BoxChecker checker(m_bbox_min, m_bbox_max);
    Vector3F p(1.0, 0.2, 0.1);
    ASSERT_FALSE(checker.is_inside(p));
    ASSERT_FALSE(checker.is_outside(p));
    ASSERT_TRUE(checker.is_on_boundary(p));
    ASSERT_FALSE(checker.is_on_boundary_edges(p));
    ASSERT_FALSE(checker.is_on_boundary_corners(p));
}

TEST_F(BoxCheckerTest, mid_edge_point) {
    BoxChecker checker(m_bbox_min, m_bbox_max);
    Vector3F p(1.0, -1.0, 0.1);
    ASSERT_FALSE(checker.is_inside(p));
    ASSERT_FALSE(checker.is_outside(p));
    ASSERT_TRUE(checker.is_on_boundary(p));
    ASSERT_TRUE(checker.is_on_boundary_edges(p));
    ASSERT_FALSE(checker.is_on_boundary_corners(p));
}

TEST_F(BoxCheckerTest, corner_point) {
    BoxChecker checker(m_bbox_min, m_bbox_max);
    Vector3F p(1.0, 1.0, 1.0);
    ASSERT_FALSE(checker.is_inside(p));
    ASSERT_FALSE(checker.is_outside(p));
    ASSERT_TRUE(checker.is_on_boundary(p));
    ASSERT_TRUE(checker.is_on_boundary_edges(p));
    ASSERT_TRUE(checker.is_on_boundary_corners(p));
}
