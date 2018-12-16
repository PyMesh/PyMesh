/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE

#include "../TriangulationTest.h"

class TriangleConformingDelaunayTest : public TriangulationTest {
};

TEST_F(TriangleConformingDelaunayTest, simple) {
    MatrixFr points(3, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              0.0, 1.0;

    Triangulation::Ptr t = Triangulation::create("triangle_conforming_delaunay");
    t->set_points(points);
    t->run();
    MatrixIr faces = t->get_faces();
    ASSERT_EQ(1, faces.rows());
    ASSERT_TRUE(
            (faces(0, 0) == 0 && faces(0, 1) == 1 && faces(0, 2) == 2) ||
            (faces(0, 0) == 1 && faces(0, 1) == 2 && faces(0, 2) == 0) ||
            (faces(0, 0) == 2 && faces(0, 1) == 0 && faces(0, 2) == 1));
    assert_valid_triangulation(points, faces);
}

TEST_F(TriangleConformingDelaunayTest, simple2) {
    MatrixFr points(4, 2);
    points << 0.0, 0.0,
              0.5, 0.0,
              1.0, 0.0,
              0.0, 1.0;

    Triangulation::Ptr t = Triangulation::create("triangle_conforming_delaunay");
    t->set_points(points);
    t->run();
    MatrixFr vertices = t->get_vertices();
    MatrixIr faces = t->get_faces();
    assert_valid_triangulation(vertices, faces);
}

TEST_F(TriangleConformingDelaunayTest, simple3) {
    MatrixFr points(4, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              1.0, 1.0,
              0.0, 1.0;

    Triangulation::Ptr t = Triangulation::create("triangle_conforming_delaunay");
    t->set_points(points);
    t->run();
    MatrixFr vertices = t->get_vertices();
    MatrixIr faces = t->get_faces();
    ASSERT_EQ(2, faces.rows());
    assert_valid_triangulation(vertices, faces);
}

TEST_F(TriangleConformingDelaunayTest, DISABLED_degenerate) {
    // Note: Triangle's conforming Delaunay seg faults on degenerate input.
    MatrixFr points(3, 2);
    points << 0.0, 0.0,
              0.5, 0.0,
              1.0, 0.0;

    Triangulation::Ptr t = Triangulation::create("triangle_conforming_delaunay");
    t->set_points(points);
    t->run();
    MatrixIr faces = t->get_faces();
    ASSERT_EQ(0, faces.rows());
}

TEST_F(TriangleConformingDelaunayTest, intersecting_constraints) {
    MatrixFr points(4, 2);
    points << 1.0, 0.0,
             -1.0, 0.0,
              0.0, 1.0,
              0.0,-1.0;
    Matrix2Ir segments(2, 2);
    segments << 0, 1,
                2, 3;

    Triangulation::Ptr t = Triangulation::create("triangle_conforming_delaunay");
    t->set_points(points);
    t->set_segments(segments);
    t->run();

    MatrixFr vertices = t->get_vertices();
    MatrixIr faces = t->get_faces();
    assert_valid_triangulation(vertices, faces);
    ASSERT_LE(5, vertices.rows());
    ASSERT_LE(4, faces.rows());
}

TEST_F(TriangleConformingDelaunayTest, overlapping_segments) {
    MatrixFr points(6, 2);
    points << 1.0, 0.0,
             -1.0, 0.0,
              2.0, 0.0,
             -2.0, 0.0,
              0.0, 1.0,
              0.0,-1.0;
    Matrix2Ir segments(3, 2);
    segments << 0, 1,
                2, 3,
                4, 5;

    Triangulation::Ptr t = Triangulation::create("triangle_conforming_delaunay");
    t->set_points(points);
    t->set_segments(segments);
    t->run();

    MatrixFr vertices = t->get_vertices();
    MatrixIr faces = t->get_faces();
    assert_valid_triangulation(vertices, faces);
    ASSERT_LE(7, vertices.rows());
    ASSERT_LE(8, faces.rows());
}

TEST_F(TriangleConformingDelaunayTest, near_degeneracy) {
    MatrixFr points(4, 2);
    points << -1.0, 0.0,
               1.0, 0.0,
               0.0, 1.0,
               0.0, 0.0;
    Matrix2Ir segments(2, 2);
    segments << 0, 1,
                2, 3;
    Triangulation::Ptr t = Triangulation::create("triangle_constrained_delaunay");

    Float tol = 1e-3;
    for (size_t i=0; i<10; i++) {
        points(3, 1) = tol;
        t->set_points(points);
        t->set_segments(segments);
        t->run();

        MatrixFr vertices = t->get_vertices();
        MatrixIr faces = t->get_faces();
        assert_valid_triangulation(vertices, faces);
        ASSERT_LE(4, vertices.rows());
        ASSERT_LE(3, faces.rows());

        tol /= 10.0;
    }
}

#endif
