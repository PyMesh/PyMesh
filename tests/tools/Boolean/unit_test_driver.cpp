/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Cork/CorkEngineTest.h"
#include "Clipper/ClipperEngineTest.h"
#include "IGL/IGLEngineTest.h"
#include "IGL/IGLCSGTreeTest.h"
#include "CGAL/CGALBooleanEngineTest.h"
#include "CGAL/CGALCorefinementEngineTest.h"
#include "Carve/CarveEngineTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
