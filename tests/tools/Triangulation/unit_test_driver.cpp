/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include <gtest/gtest.h>
#ifdef WITH_IGL
#include "IGL/LexicographicTriangulationTest.h"
#ifdef WITH_CGAL
#include "IGL/DelaunayTriangulationTest.h"
#include "CGAL/CGALDelaunayTriangulationTest.h"
#endif
#endif
#ifdef WITH_TRIANGLE
#include "ShewchukTriangle/ShewchukTriangleTest.h"
#endif
#ifdef WITH_GEOGRAM
#include "GeoGram/GeoGramDelaunayTriangulationTest.h"
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
