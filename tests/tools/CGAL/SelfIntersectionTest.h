/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>
#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <SelfIntersection.h>

class SelfIntersectionTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            MatrixFr vertices(8, 3);
            vertices << 0, 0, 0,
                        0, 1, 0,
                        1, 0, 0,
                        1, 1, 0,
                        2, 0, 0,
                        2, 1, 0,
                        0, 0, 1,
                        0, 1, 0;
            m_vertices = vertices;
        }

        MatrixIr check_self_intersection(
                const MatrixFr& vertices, const MatrixIr& faces) {
            SelfIntersection detector(vertices, faces);
            detector.detect_self_intersection();
            return detector.get_self_intersecting_pairs();
        }

    protected:
        MatrixFr m_vertices;
};

TEST_F(SelfIntersectionTest, NoTouching) {
    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             3, 4, 5;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(0, result.rows());
}

TEST_F(SelfIntersectionTest, NoTouchingButClose) {
    MatrixI faces(2, 3);
    faces << 0, 1, 2,
             3, 4, 5;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(0, result.rows());
}

TEST_F(SelfIntersectionTest, PointTouching) {
    MatrixI faces(2, 3);
    faces << 0, 1, 2,
             3, 7, 4;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(1, result.rows());
}

TEST_F(SelfIntersectionTest, EdgeTouching) {
    MatrixI faces(2, 3);
    faces << 0, 2, 1,
             3, 7, 2;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(1, result.rows());
}

TEST_F(SelfIntersectionTest, OverLapping) {
    MatrixI faces(2, 3);
    faces << 0, 2, 1,
             0, 5, 3;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(1, result.rows());
}

TEST_F(SelfIntersectionTest, IdentiticalTriangles) {
    MatrixI faces(2, 3);
    faces << 0, 3, 5,
             0, 5, 3;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(1, result.rows());
}

TEST_F(SelfIntersectionTest, SingleDegeneratedTriangles) {
    // Degenerated faces are skipped.
    MatrixI faces(2, 3);
    faces << 0, 3, 3,
             1, 2, 4;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(0, result.rows());
}

TEST_F(SelfIntersectionTest, DoubleDegeneratedTriangles) {
    // Degenerated faces are skipped.
    MatrixI faces(2, 3);
    faces << 0, 5, 5,
             1, 2, 2;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(0, result.rows());
}

TEST_F(SelfIntersectionTest, PointLikeTriangles) {
    // Degenerated faces are skipped.
    MatrixI faces(2, 3);
    faces << 2, 2, 2,
             0, 4, 3;
    MatrixIr result = check_self_intersection(m_vertices, faces);
    ASSERT_EQ(0, result.rows());
}
