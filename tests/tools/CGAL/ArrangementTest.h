/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <TestBase.h>
#include <CGAL/Arrangement2.h>

class Arrangement2Test : public TestBase {
};

TEST_F(Arrangement2Test, simple) {
    Matrix2Fr points(3, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              0.0, 1.0;
    Matrix2Ir segments(3, 2);
    segments << 0, 1,
                1, 2,
                2, 0;

    Arrangement2 arrangement;
    arrangement.set_points(points);
    arrangement.set_segments(segments);
    arrangement.run();

    const auto& vertices = arrangement.get_vertices();
    const auto& edges = arrangement.get_edges();

    ASSERT_EQ(3, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_EQ(3, edges.rows());
    ASSERT_EQ(2, edges.cols());

    Matrix2Fr queries(4, 2);
    queries << 0.0, 0.0,
               0.5, 0.0,
               0.5, 0.1,
               1.0, 1.0;
    const auto r = arrangement.query(queries);
    ASSERT_EQ(Arrangement2::POINT,  r[0].first);
    ASSERT_EQ(Arrangement2::SEGMENT,r[1].first);
    ASSERT_EQ(Arrangement2::CELL,   r[2].first);
    ASSERT_EQ(Arrangement2::CELL,   r[3].first);

    ASSERT_DOUBLE_EQ(0, (vertices.row(r[0].second) - points.row(0)).norm());
    ASSERT_NE(r[2].second, r[3].second);
}

TEST_F(Arrangement2Test, double_cell) {
    Matrix2Fr points(4, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              1.0, 1.0,
              0.0, 1.0;
    Matrix2Ir segments(5, 2);
    segments << 0, 1,
                1, 2,
                2, 3,
                3, 0,
                1, 3 ;

    Arrangement2 arrangement;
    arrangement.set_points(points);
    arrangement.set_segments(segments);
    arrangement.run();

    const auto& vertices = arrangement.get_vertices();
    const auto& edges = arrangement.get_edges();

    ASSERT_EQ(points.rows(), vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_EQ(segments.rows(), edges.rows());
    ASSERT_EQ(2, edges.cols());

    Matrix2Fr queries(6, 2);
    queries << 0.0, 0.0,
               0.5, 0.0,
               0.5, 0.1,
               0.1, 0.1,
               0.8, 0.8,
               0.9, 0.9;
    const auto r = arrangement.query(queries);
    ASSERT_EQ(Arrangement2::POINT,  r[0].first);
    ASSERT_EQ(Arrangement2::SEGMENT,r[1].first);
    ASSERT_EQ(Arrangement2::CELL,   r[2].first);
    ASSERT_EQ(Arrangement2::CELL,   r[3].first);
    ASSERT_EQ(Arrangement2::CELL,   r[4].first);
    ASSERT_EQ(Arrangement2::CELL,   r[5].first);

    ASSERT_DOUBLE_EQ(0, (vertices.row(r[0].second) - points.row(0)).norm());
    ASSERT_EQ(r[2].second, r[3].second);
    ASSERT_EQ(r[4].second, r[5].second);
}


#endif
