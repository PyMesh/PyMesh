#include <gtest/gtest.h>
#include "BoundingBoxTest.h"
#include "AABBTreeTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
