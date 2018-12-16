/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include "../TriangulationTest.h"

class ConstrainedDelaunayTest : public TriangulationTest {
};

TEST_F(ConstrainedDelaunayTest, simple) {
    simple("cgal_constrained_delaunay");
}

TEST_F(ConstrainedDelaunayTest, simple2) {
    simple2("cgal_constrained_delaunay");
}

TEST_F(ConstrainedDelaunayTest, simple3) {
    simple3("cgal_constrained_delaunay");
}

TEST_F(ConstrainedDelaunayTest, degenerate) {
    degenerate("cgal_constrained_delaunay");
}

TEST_F(ConstrainedDelaunayTest, intersecting_constraints) {
    intersecting_constraints("cgal_constrained_delaunay");
}

TEST_F(ConstrainedDelaunayTest, overlapping_segments) {
    overlapping_segments("cgal_constrained_delaunay");
}

TEST_F(ConstrainedDelaunayTest, near_degeneracy) {
    near_degeneracy("cgal_constrained_delaunay");
}

#endif
