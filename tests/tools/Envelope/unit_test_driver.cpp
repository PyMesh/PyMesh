#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "GridTest.h"
#include "VoxelGridTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
