#include <gtest/gtest.h>
#include "DimReductionTest.h"
#include "TriangleWrapperTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
