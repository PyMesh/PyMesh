#include <gtest/gtest.h>
#include "eigen_pass_matrix_without_copy_test.h"
#include "eigen_quaternion_test.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
