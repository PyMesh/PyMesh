#pragma once

#include <iostream>
#include <string>
#include <Wires/Inflator/GeometryCorrectionTable.h>
#include <WireTest.h>

class GeometryCorrectionTableTest : public WireTest {
    protected:
};

TEST_F(GeometryCorrectionTableTest, extrapolate) {
    std::string correction_file = m_data_dir + "geometry.csv";
    GeometryCorrectionTable table(correction_file);
    MatrixFr loop(4, 3);
    loop << -0.1, -0.1, -0.1,
             0.1, -0.1, -0.1,
             0.1,  0.1, -0.1,
            -0.1,  0.1, -0.1;
    MatrixFr ori_loop = loop;
    Vector3F edge_dir(0, 0, 1);

    table.apply_correction(edge_dir, loop);
    ASSERT_NEAR(0.0, (loop - ori_loop).norm(), 1e-6);
}

TEST_F(GeometryCorrectionTableTest, extrapolate1) {
    std::string correction_file = m_data_dir + "geometry.csv";
    GeometryCorrectionTable table(correction_file);
    MatrixFr loop(4, 3);
    loop << -2.1, -2.1, -0.1,
             2.1, -2.1, -0.1,
             2.1,  2.1, -0.1,
            -2.1,  2.1, -0.1;
    Vector3F edge_dir(0, 0, 1);

    table.apply_correction(edge_dir, loop);
    ASSERT_FLOAT_EQ(-1.6, loop(0, 1));
    ASSERT_FLOAT_EQ(-1.6, loop(1, 1));
    ASSERT_FLOAT_EQ( 1.6, loop(2, 1));
    ASSERT_FLOAT_EQ( 1.6, loop(3, 1));
}
