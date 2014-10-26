#pragma once

#include <triangle/TriangleWrapper.h>

class TriangleWrapperTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            m_vertices.resize(8, 2);
            m_vertices << 0.0, 0.0,
                          1.0, 0.0,
                          1.0, 1.0,
                          0.0, 1.0,
                          0.2, 0.2,
                          0.8, 0.2,
                          0.8, 0.8,
                          0.2, 0.8;

            m_segments.resize(8, 2);
            m_segments << 0, 1,
                          1, 2,
                          2, 3,
                          3, 0,
                          4, 5,
                          5, 6,
                          6, 7,
                          7, 0;
            m_holes.resize(1, 2);
            m_holes << 0.5, 0.5;
        }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_segments;
        MatrixFr m_holes;
};

TEST_F(TriangleWrapperTest, Creation) {
    TriangleWrapper tri(m_vertices, m_segments);
    tri.run(0.1, true);

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), m_vertices.rows());
    ASSERT_GT(faces.rows(), 0);

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_FLOAT_EQ(0.0, bbox_min.minCoeff());
    ASSERT_FLOAT_EQ(0.0, bbox_min.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.minCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.maxCoeff());
}

TEST_F(TriangleWrapperTest, Holes) {
    TriangleWrapper tri(m_vertices, m_segments);
    tri.set_holes(m_holes);
    tri.run(0.1, true);

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), m_vertices.rows());
    ASSERT_GT(faces.rows(), 0);

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_FLOAT_EQ(0.0, bbox_min.minCoeff());
    ASSERT_FLOAT_EQ(0.0, bbox_min.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.minCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.maxCoeff());
}

TEST_F(TriangleWrapperTest, 3D) {
    size_t num_pts = m_vertices.rows();
    MatrixFr vertices_3d(num_pts, 3);
    vertices_3d.block(0,0,num_pts,2) = m_vertices;
    vertices_3d.block(0,2,num_pts,1) = VectorF::Ones(num_pts);

    TriangleWrapper tri(vertices_3d, m_segments);
    tri.run(0.1, true);

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), num_pts);
    ASSERT_EQ(3, vertices.cols());
    ASSERT_GT(faces.rows(), 0);

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_FLOAT_EQ(0.0, bbox_min[0]);
    ASSERT_FLOAT_EQ(0.0, bbox_min[1]);
    ASSERT_FLOAT_EQ(1.0, bbox_max[0]);
    ASSERT_FLOAT_EQ(1.0, bbox_max[1]);
}
