/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include "../TriangulationTest.h"

class ConformingDelaunayTest : public TriangulationTest {
};

TEST_F(ConformingDelaunayTest, simple) {
    simple("cgal_conforming_delaunay");
}

TEST_F(ConformingDelaunayTest, simple2) {
    simple2("cgal_conforming_delaunay");
}

TEST_F(ConformingDelaunayTest, simple3) {
    simple3("cgal_conforming_delaunay");
}

TEST_F(ConformingDelaunayTest, degenerate) {
    degenerate("cgal_conforming_delaunay");
}

TEST_F(ConformingDelaunayTest, intersecting_constraints) {
    intersecting_constraints("cgal_conforming_delaunay");
}

TEST_F(ConformingDelaunayTest, overlapping_segments) {
    overlapping_segments("cgal_conforming_delaunay");
}

TEST_F(ConformingDelaunayTest, near_degeneracy) {
    near_degeneracy("cgal_conforming_delaunay");
}

#endif
