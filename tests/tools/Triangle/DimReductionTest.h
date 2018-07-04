/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <triangle/DimReduction.h>
#include <TestBase.h>

class DimReductionTest : public TestBase {
};

TEST_F(DimReductionTest, 3Dto2D) {
    MatrixFr pts(4, 3);
    pts << 0.0, 0.0, 0.0,
           1.0, 0.0, 2.0,
           2.0, 0.0, 1.0,
           4.0, 0.0, -1.0;
    DimReduction<3, 2> reductor(pts);

    MatrixFr proj_pts = reductor.project(pts);
    MatrixFr unproj_pts = reductor.unproject(proj_pts);

    ASSERT_EQ(pts.rows(), proj_pts.rows());
    ASSERT_EQ(pts.rows(), unproj_pts.rows());
    ASSERT_NEAR(0.0, (pts - unproj_pts).norm(), 1e-12);
}

TEST_F(DimReductionTest, 3Dto1D) {
    MatrixFr pts(4, 3);
    pts << 0.0, 10.0, 0.0,
           1.0, 10.0, 2.0,
           2.0, 10.0, 4.0,
          -1.0, 10.0, -2.0;
    DimReduction<3, 1> reductor(pts);

    MatrixFr proj_pts = reductor.project(pts);
    MatrixFr unproj_pts = reductor.unproject(proj_pts);

    ASSERT_EQ(pts.rows(), proj_pts.rows());
    ASSERT_EQ(pts.rows(), unproj_pts.rows());
    ASSERT_NEAR(0.0, (pts - unproj_pts).norm(), 1e-12);
}
