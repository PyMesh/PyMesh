#pragma once

#include <EigenTypedef.h>
#include <Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <BoundingBox.h>

class BoundingBoxTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            MatrixF vertices(7, 3);
            vertices << 0, 0, 0,
                        0, 1, 0,
                        1, 0, 0,
                        1, 1, 0,
                        2, 0, 0,
                        2, 1, 0,
                        0, 0, 1;
            m_vertices = vertices;
        }

        virtual void TearDown() {
        }

    protected:
        MatrixF m_vertices;
};

TEST_F(BoundingBoxTest, NoTouching) {
    MatrixI faces(2, 3);
    faces << 0, 1, 2,
             3, 4, 5;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(0.1);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(0, result.rows());
}

TEST_F(BoundingBoxTest, NoTouchingButClose) {
    MatrixI faces(2, 3);
    faces << 0, 1, 2,
             3, 4, 5;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(0.8);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
}

TEST_F(BoundingBoxTest, PointTouching) {
    MatrixI faces(2, 3);
    faces << 0, 1, 2,
             3, 2, 4;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(1e-12);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
    ASSERT_EQ(1, result[0]);
}

TEST_F(BoundingBoxTest, EdgeTouching) {
    MatrixI faces(2, 3);
    faces << 0, 2, 1,
             3, 1, 2;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(1e-12);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
    ASSERT_EQ(1, result[0]);
}

TEST_F(BoundingBoxTest, OverLapping) {
    MatrixI faces(2, 3);
    faces << 0, 2, 1,
             0, 5, 3;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(1e-12);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
    ASSERT_EQ(1, result[0]);
}

TEST_F(BoundingBoxTest, IdentiticalTriangles) {
    MatrixI faces(2, 3);
    faces << 0, 3, 5,
             0, 5, 3;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(1e-12);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
    ASSERT_EQ(1, result[0]);
}

TEST_F(BoundingBoxTest, SingleDegeneratedTriangles) {
    MatrixI faces(2, 3);
    faces << 0, 3, 3,
             1, 2, 4;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(1e-12);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
    ASSERT_EQ(1, result[0]);
}

TEST_F(BoundingBoxTest, DoubleDegeneratedTriangles) {
    MatrixI faces(2, 3);
    faces << 0, 5, 5,
             1, 2, 2;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(1e-12);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
    ASSERT_EQ(1, result[0]);
}

TEST_F(BoundingBoxTest, PointLikeTriangles) {
    MatrixI faces(2, 3);
    faces << 2, 2, 2,
             0, 4, 3;
    BoundingBoxes bboxes = BoundingBoxes(m_vertices, faces);
    bboxes.check_touching_faces(1e-12);

    VectorI result = bboxes.get_touching_face_indices(0);
    ASSERT_EQ(1, result.rows());
    ASSERT_EQ(1, result[0]);
}
