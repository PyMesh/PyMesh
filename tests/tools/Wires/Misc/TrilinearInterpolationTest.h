/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <cmath>
#include <vector>

#include <Wires/Misc/TrilinearInterpolation.h>

class TrilinearInterpolationTest : public ::testing::Test {
    protected:
        void SetUp() {
            m_corners.resize(8, 3);
            m_corners << 0.0, 0.0, 0.0,
                         2.0, 0.0, 0.0,
                         2.0, 2.0, 0.0,
                         0.0, 2.0, 0.0,
                         0.0, 0.0, 2.0,
                         2.0, 0.0, 2.0,
                         2.0, 2.0, 2.0,
                         0.0, 2.0, 2.0;

            m_pts.push_back(Vector3F(0.0, 0.0, 0.0));
            m_pts.push_back(Vector3F(1.0, 0.0, 0.0));
            m_pts.push_back(Vector3F(1.0, 1.0, 0.0));
            m_pts.push_back(Vector3F(0.0, 1.0, 0.0));
            m_pts.push_back(Vector3F(0.0, 0.0, 1.0));
            m_pts.push_back(Vector3F(1.0, 0.0, 1.0));
            m_pts.push_back(Vector3F(1.0, 1.0, 1.0));
            m_pts.push_back(Vector3F(0.0, 1.0, 1.0));
            m_pts.push_back(Vector3F(0.5, 0.5, 0.5));
            m_pts.push_back(Vector3F(0.25, 0.75, 0.5));

            m_targets.push_back(Vector3F(0.0, 0.0, 0.0));
            m_targets.push_back(Vector3F(2.0, 0.0, 0.0));
            m_targets.push_back(Vector3F(2.0, 2.0, 0.0));
            m_targets.push_back(Vector3F(0.0, 2.0, 0.0));
            m_targets.push_back(Vector3F(0.0, 0.0, 2.0));
            m_targets.push_back(Vector3F(2.0, 0.0, 2.0));
            m_targets.push_back(Vector3F(2.0, 2.0, 2.0));
            m_targets.push_back(Vector3F(0.0, 2.0, 2.0));
            m_targets.push_back(Vector3F(1.0, 1.0, 1.0));
            m_targets.push_back(Vector3F(0.5, 1.5, 1.0));
        }

        void ASSERT_VECTOR_EQ(const VectorF& v1, const VectorF& v2) {
            ASSERT_NEAR(0.0, (v1 - v2).norm(), 1e-6);
        }

    protected:
        MatrixFr m_corners;
        std::vector<Vector3F> m_pts;
        std::vector<Vector3F> m_targets;
};

TEST_F (TrilinearInterpolationTest, Simple) {
    TrilinearInterpolation interpolator(m_corners);
    const size_t num_samples = m_pts.size();
    for (size_t i=0; i<num_samples; i++) {
        ASSERT_VECTOR_EQ(m_targets[i],
                interpolator.interpolate(m_pts[i]));
    }
}

TEST_F(TrilinearInterpolationTest, TransformedCorners) {
    MatrixF mat(3, 3);
    mat << 1.0, 2.0, 0.5,
           0.0, 0.5, 0.5,
           1.0, 0.0, 2.0;

    m_corners = (mat * m_corners.transpose()).transpose();
    TrilinearInterpolation interpolator(m_corners);
    const size_t num_samples = m_pts.size();
    for (size_t i=0; i<num_samples; i++) {
        ASSERT_VECTOR_EQ(mat * m_targets[i],
                interpolator.interpolate(m_pts[i]));
    }
}

TEST_F(TrilinearInterpolationTest, Batch) {
    const size_t num_pts = m_pts.size();
    MatrixFr pts(num_pts, 3);
    MatrixFr targets(num_pts, 3);

    for (size_t i=0; i<num_pts; i++) {
        pts.row(i) = m_pts[i];
        targets.row(i) = m_targets[i];
    }

    TrilinearInterpolation interpolator(m_corners);
    MatrixFr result = interpolator.interpolate_batch(pts);
    ASSERT_FLOAT_EQ(0.0, (result - targets).norm());
}

