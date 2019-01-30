/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include <gtest/gtest.h>
#ifdef WITH_IGL
#include "IGL/LexicographicTriangulationTest.h"
#ifdef WITH_CGAL
#include "IGL/DelaunayTriangulationTest.h"
#include "CGAL/ConstrainedDelaunayTest.h"
#include "CGAL/ConformingDelaunayTest.h"
#endif
#endif
#ifdef WITH_TRIANGLE
#include "ShewchukTriangle/ConformingDelaunayTest.h"
#include "ShewchukTriangle/ConstrainedDelaunayTest.h"
#endif
#ifdef WITH_GEOGRAM
#include "Geogram/GeogramDelaunayTriangulationTest.h"
#endif
#ifdef WITH_JIGSAW
#include "Jigsaw/FrontalDelaunayTest.h"
#endif
#ifdef WITH_MMG
#include "MMG/DelaunayTest.h"
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
