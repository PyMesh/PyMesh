/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include "../TriangulationTest.h"

class CGALConstrainedDelaunayTest : public TriangulationTest {
};

TEST_F(CGALConstrainedDelaunayTest, simple) {
    simple("cgal_constrained_delaunay");
}

TEST_F(CGALConstrainedDelaunayTest, simple2) {
    simple2("cgal_constrained_delaunay");
}

TEST_F(CGALConstrainedDelaunayTest, simple3) {
    simple3("cgal_constrained_delaunay");
}

TEST_F(CGALConstrainedDelaunayTest, degenerate) {
    degenerate("cgal_constrained_delaunay");
}

TEST_F(CGALConstrainedDelaunayTest, intersecting_constraints) {
    intersecting_constraints("cgal_constrained_delaunay");
}

TEST_F(CGALConstrainedDelaunayTest, overlapping_segments) {
    overlapping_segments("cgal_constrained_delaunay");
}

TEST_F(CGALConstrainedDelaunayTest, near_degeneracy) {
    near_degeneracy("cgal_constrained_delaunay");
}

TEST_F(CGALConstrainedDelaunayTest, duplicated_vertices) {
    duplicated_vertices("cgal_constrained_delaunay");
}

#endif
