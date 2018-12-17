/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE

#include "../TriangulationTest.h"

class TriangleConformingDelaunayTest : public TriangulationTest {
};

TEST_F(TriangleConformingDelaunayTest, simple) {
    simple("triangle_conforming_delaunay");
}

TEST_F(TriangleConformingDelaunayTest, simple2) {
    simple2("triangle_conforming_delaunay");
}

TEST_F(TriangleConformingDelaunayTest, simple3) {
    simple3("triangle_conforming_delaunay");
}

TEST_F(TriangleConformingDelaunayTest, DISABLED_degenerate) {
    degenerate("triangle_conforming_delaunay");
}

TEST_F(TriangleConformingDelaunayTest, intersecting_constraints) {
    intersecting_constraints("triangle_conforming_delaunay");
}

TEST_F(TriangleConformingDelaunayTest, overlapping_segments) {
    overlapping_segments("triangle_conforming_delaunay");
}

TEST_F(TriangleConformingDelaunayTest, near_degeneracy) {
    near_degeneracy("triangle_conforming_delaunay");
}

TEST_F(TriangleConformingDelaunayTest, duplicated_vertices) {
    duplicated_vertices("triangle_conforming_delaunay");
}

#endif
