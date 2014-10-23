#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "CGAL/CGALConvexHull2DTest.h"
#include "CGAL/CGALConvexHull3DTest.h"
#include "Qhull/QhullEngineTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
