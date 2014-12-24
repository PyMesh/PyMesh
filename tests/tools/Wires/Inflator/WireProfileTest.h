#pragma once

#include <cmath>
#include <iostream>
#include <Wires/Inflator/WireProfile.h>
#include <WireTest.h>

class WireProfileTest : public WireTest {
    protected:
        VectorF compute_center(const MatrixFr& loop) {
            VectorF bbox_min = loop.colwise().minCoeff();
            VectorF bbox_max = loop.colwise().maxCoeff();
            VectorF center = 0.5 * (bbox_min + bbox_max);
            return center;
        }

        Float compute_offset(const MatrixFr& loop, const VectorF& base_p) {
            VectorF center = compute_center(loop);
            return (center - base_p).norm();
        }

        Float compute_radius(const MatrixFr& loop) {
            VectorF center = compute_center(loop);
            return (loop.rowwise() - center.transpose()).rowwise().norm().maxCoeff();
        }

        Float compute_area(const VectorF& v1, const VectorF& v2) {
            Vector3F u(0, 0, 0);
            Vector3F v(0, 0, 0);
            u.segment(0, v1.size()) = v1;
            v.segment(0, v2.size()) = v2;
            return u.cross(v).norm();
        }

        void ASSERT_COLINEAR(const MatrixFr& loop, const VectorF& p0, const VectorF& p1) {
            Float area = compute_area(compute_center(loop)-p0, p1-p0);
            ASSERT_NEAR(0.0, area, 1e-6);
        }
};

TEST_F(WireProfileTest, 3D) {
    const Float EPS = 1e-6;
    WireProfile::Ptr profile = WireProfile::create("square");
    Vector3F p0(0,0,0);
    Vector3F p1(1,0,0);
    Float offset = 0.0;
    Float thickness = 1.0;
    Vector3F correction(0, 0, 0);
    MatrixFr loop = profile->place(p0, p1, offset, thickness, correction);
    ASSERT_FLOAT_EQ(offset, compute_offset(loop, p0));
    ASSERT_FLOAT_EQ(thickness * 0.5, compute_radius(loop));
    ASSERT_COLINEAR(loop, p0, p1);
}

TEST_F(WireProfileTest, 3D_diag) {
    WireProfile::Ptr profile = WireProfile::create("square");
    Vector3F p0(0,0,0);
    Vector3F p1(1,1,1);
    Float offset = 0.5;
    Float thickness = 2.0;
    Vector3F correction(0, 0, 0);
    MatrixFr loop = profile->place(p0, p1, offset, thickness, correction);
    ASSERT_FLOAT_EQ(offset, compute_offset(loop, p0));
    ASSERT_FLOAT_EQ(thickness * 0.5, compute_radius(loop));
    ASSERT_COLINEAR(loop, p0, p1);
}

TEST_F(WireProfileTest, 2D) {
    WireProfile::Ptr profile = WireProfile::create_2D();
    Vector2F p0(0,0);
    Vector2F p1(1,0);
    Float offset = 0.3;
    Float thickness = 1.1;
    Vector2F correction(0, 0);
    MatrixFr loop = profile->place(p0, p1, offset, thickness, correction);
    ASSERT_FLOAT_EQ(offset, compute_offset(loop, p0));
    ASSERT_FLOAT_EQ(thickness * 0.5, compute_radius(loop));
    ASSERT_COLINEAR(loop, p0, p1);
}

TEST_F(WireProfileTest, 2D_diag) {
    WireProfile::Ptr profile = WireProfile::create_2D();
    Vector2F p0(0,0);
    Vector2F p1(1,1);
    Float offset = 0.1;
    Float thickness = 1.0;
    Vector2F correction(0, 0);
    MatrixFr loop = profile->place(p0, p1, offset, thickness, correction);
    ASSERT_FLOAT_EQ(offset, compute_offset(loop, p0));
    ASSERT_FLOAT_EQ(thickness * 0.5, compute_radius(loop));
    ASSERT_COLINEAR(loop, p0, p1);
}
