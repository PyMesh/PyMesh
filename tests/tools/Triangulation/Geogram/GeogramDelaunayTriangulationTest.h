/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_GEOGRAM

#include "../TriangulationTest.h"

class GeogramDelaunayTriangulationTest : public TriangulationTest {
};

TEST_F(GeogramDelaunayTriangulationTest, simple) {
    simple("geogram_delaunay");
}

TEST_F(GeogramDelaunayTriangulationTest, simple2) {
    simple2("geogram_delaunay");
}

TEST_F(GeogramDelaunayTriangulationTest, simple3) {
    simple3("geogram_delaunay");
}

TEST_F(GeogramDelaunayTriangulationTest, degenerate) {
    degenerate("geogram_delaunay");
}

#endif
