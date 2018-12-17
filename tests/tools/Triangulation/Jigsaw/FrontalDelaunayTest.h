/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_JIGSAW

#include "../TriangulationTest.h"

class JigsawFrontalDelaunayTest : public TriangulationTest {
};

TEST_F(JigsawFrontalDelaunayTest, square) {
    Matrix2Fr points(4, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              1.0, 1.0,
              0.0, 1.0;
    Matrix2Ir segments(4, 2);
    segments << 0, 1,
                1, 2,
                2, 3,
                3, 0;

    Triangulation::Ptr t = Triangulation::create("jigsaw_frontal_delaunay");
    t->set_points(points);
    t->set_segments(segments);
    t->run();

    MatrixFr vertices = t->get_vertices();
    MatrixIr faces = t->get_faces();
    assert_valid_triangulation(vertices, faces);
    ASSERT_LE(4, vertices.rows());
    ASSERT_LE(2, faces.rows());
}

TEST_F(JigsawFrontalDelaunayTest, self_intersection) {
    Matrix2Fr points(4, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              1.0, 1.0,
              0.0, 1.0;
    Matrix2Ir segments(6, 2);
    segments << 0, 1,
                1, 2,
                2, 3,
                3, 0,
                0, 2,
                1, 3;

    Triangulation::Ptr t = Triangulation::create("jigsaw_frontal_delaunay");
    t->set_points(points);
    t->set_segments(segments);
    t->run();

    MatrixFr vertices = t->get_vertices();
    MatrixIr faces = t->get_faces();
    assert_valid_triangulation(vertices, faces);
    ASSERT_LE(3, vertices.rows());
    ASSERT_LE(1, faces.rows());
}

TEST_F(JigsawFrontalDelaunayTest, duplicated_vertices) {
    duplicated_vertices("jigsaw_frontal_delaunay");
}

#endif
