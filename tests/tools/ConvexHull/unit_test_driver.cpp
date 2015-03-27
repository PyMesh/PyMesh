/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#ifdef WITH_CGAL
#include "CGAL/CGALConvexHull2DTest.h"
#include "CGAL/CGALConvexHull3DTest.h"
#endif
#ifdef WITH_QHULL
#include "Qhull/QhullEngineTest.h"
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
