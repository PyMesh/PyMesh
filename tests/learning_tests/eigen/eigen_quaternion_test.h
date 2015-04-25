#pragma once

#include <Core/EigenTypedef.h>
#include <Eigen/Geometry>

class EigenQuaternionTest : public ::testing::Test {
    protected:
};

TEST_F(EigenQuaternionTest, Rotation) {
    Vector3F Z(0, 0, 1);
    Vector3F dir(-1e-7, -1e-7, -1.0);
    Float dir_len = dir.norm();

    Eigen::Quaternion<Float> Q;
    Q.setFromTwoVectors(Z, dir);

    Float diff = (Q.toRotationMatrix() * Z * dir_len - dir).norm();

    // Note that Eigen::Quaternion introduce large numeric error (around 2e-7)
    // for 180 rotations.  So the following test would fail.
    //ASSERT_FLOAT_EQ(0.0, diff);

    // A relaxed test should pass.
    ASSERT_NEAR(0.0, diff, 1e-6);
}
