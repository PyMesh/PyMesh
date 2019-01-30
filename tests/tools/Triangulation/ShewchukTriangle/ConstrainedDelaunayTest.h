/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE

#include "../TriangulationTest.h"

class TriangleConstrainedDelaunayTest : public TriangulationTest {
};

TEST_F(TriangleConstrainedDelaunayTest, simple) {
    simple("triangle_constrained_delaunay");
}

TEST_F(TriangleConstrainedDelaunayTest, simple2) {
    simple2("triangle_constrained_delaunay");
}

TEST_F(TriangleConstrainedDelaunayTest, simple3) {
    simple3("triangle_constrained_delaunay");
}

TEST_F(TriangleConstrainedDelaunayTest, DISABLED_degenerate) {
    degenerate("triangle_constrained_delaunay");
}

TEST_F(TriangleConstrainedDelaunayTest, intersecting_constraints) {
    intersecting_constraints("triangle_constrained_delaunay");
}

TEST_F(TriangleConstrainedDelaunayTest, overlapping_segments) {
    overlapping_segments("triangle_constrained_delaunay");
}

TEST_F(TriangleConstrainedDelaunayTest, near_degeneracy) {
    near_degeneracy("triangle_constrained_delaunay");
}

TEST_F(TriangleConstrainedDelaunayTest, duplicated_vertices) {
    duplicated_vertices("triangle_constrained_delaunay");
}

#endif
