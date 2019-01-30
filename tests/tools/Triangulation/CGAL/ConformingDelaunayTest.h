/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include "../TriangulationTest.h"

class CGALConformingDelaunayTest : public TriangulationTest {
};

TEST_F(CGALConformingDelaunayTest, simple) {
    simple("cgal_conforming_delaunay");
}

TEST_F(CGALConformingDelaunayTest, simple2) {
    simple2("cgal_conforming_delaunay");
}

TEST_F(CGALConformingDelaunayTest, simple3) {
    simple3("cgal_conforming_delaunay");
}

TEST_F(CGALConformingDelaunayTest, degenerate) {
    degenerate("cgal_conforming_delaunay");
}

TEST_F(CGALConformingDelaunayTest, intersecting_constraints) {
    intersecting_constraints("cgal_conforming_delaunay");
}

TEST_F(CGALConformingDelaunayTest, overlapping_segments) {
    overlapping_segments("cgal_conforming_delaunay");
}

TEST_F(CGALConformingDelaunayTest, near_degeneracy) {
    near_degeneracy("cgal_conforming_delaunay");
}

TEST_F(CGALConformingDelaunayTest, duplicated_vertices) {
    duplicated_vertices("cgal_conforming_delaunay");
}

#endif
