/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include <gtest/gtest.h>
#include "DMATTest.h"
#ifdef WITH_IGL_AND_CGAL
#include "MinkowskiSumTest.h"
#endif
#ifdef WITH_IGL
#include "DiskCutter.h"
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
