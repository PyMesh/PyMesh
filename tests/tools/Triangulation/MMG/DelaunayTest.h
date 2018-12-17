/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include "../TriangulationTest.h"

class MMGDelaunayTest : public TriangulationTest {
};

TEST_F(MMGDelaunayTest, DISABLED_intersecting_constraints) {
    // MMG hangs for this input.
    intersecting_constraints("mmg_delaunay");
}

TEST_F(MMGDelaunayTest, overlapping_segments) {
    overlapping_segments("mmg_delaunay");
}

TEST_F(MMGDelaunayTest, DISABLED_near_degeneracy) {
    near_degeneracy("mmg_delaunay");
}

TEST_F(MMGDelaunayTest, duplicated_vertices) {
    duplicated_vertices("mmg_delaunay");
}

#endif
