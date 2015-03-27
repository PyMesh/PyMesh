/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <cmath>
#include <vector>

#include <Wires/Misc/BilinearInterpolation.h>

class BilinearInterpolationTest : public ::testing::Test {
    protected:
        void SetUp() {
            m_corners.resize(4, 2);
            m_corners << 0.0, 0.0,
                         2.0, 0.0,
                         2.0, 2.0,
                         0.0, 2.0;

            m_pts.push_back(Vector2F(0.0, 0.0));
            m_pts.push_back(Vector2F(1.0, 0.0));
            m_pts.push_back(Vector2F(1.0, 1.0));
            m_pts.push_back(Vector2F(0.0, 1.0));
            m_pts.push_back(Vector2F(0.5, 0.5));
            m_pts.push_back(Vector2F(0.25, 0.75));

            m_targets.push_back(Vector2F(0.0, 0.0));
            m_targets.push_back(Vector2F(2.0, 0.0));
            m_targets.push_back(Vector2F(2.0, 2.0));
            m_targets.push_back(Vector2F(0.0, 2.0));
            m_targets.push_back(Vector2F(1.0, 1.0));
            m_targets.push_back(Vector2F(0.5, 1.5));
        }

        void ASSERT_VECTOR_EQ(const VectorF& v1, const VectorF& v2) {
            ASSERT_NEAR(0.0, (v1 - v2).norm(), 1e-6);
        }

    protected:
        MatrixFr m_corners;
        std::vector<Vector2F> m_pts;
        std::vector<Vector2F> m_targets;
};

TEST_F (BilinearInterpolationTest, Simple) {
    BilinearInterpolation interpolator(m_corners);
    const size_t num_samples = m_pts.size();
    for (size_t i=0; i<num_samples; i++) {
        ASSERT_VECTOR_EQ(m_targets[i],
                interpolator.interpolate(m_pts[i]));
    }
}

TEST_F(BilinearInterpolationTest, RotatedCorners) {
    const Float angle = M_PI/5.0;
    MatrixF rot(2, 2);
    rot << cos(angle), -sin(angle),
           sin(angle),  cos(angle);

    m_corners = (rot * m_corners.transpose()).transpose();
    BilinearInterpolation interpolator(m_corners);
    const size_t num_samples = m_pts.size();
    for (size_t i=0; i<num_samples; i++) {
        ASSERT_VECTOR_EQ(rot * m_targets[i],
                interpolator.interpolate(m_pts[i]));
    }
}

TEST_F(BilinearInterpolationTest, Batch) {
    const size_t num_pts = m_pts.size();
    MatrixFr pts(num_pts, 2);
    MatrixFr targets(num_pts, 2);

    for (size_t i=0; i<num_pts; i++) {
        pts.row(i) = m_pts[i];
        targets.row(i) = m_targets[i];
    }

    BilinearInterpolation interpolator(m_corners);
    MatrixFr result = interpolator.interpolate_batch(pts);
    ASSERT_FLOAT_EQ(0.0, (result - targets).norm());
}

