/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include <gtest/gtest.h>
#include "SelfIntersectionTest.h"
#include "AABBTreeTest.h"
#include "AABBTree2Test.h"
#include "ArrangementTest.h"
#include "StraightSkeletonTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
