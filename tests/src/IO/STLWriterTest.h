/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>

#include <IO/MeshWriter.h>
#include "WriterTest.h"

class STLWriterTest : public WriterTest {
};

TEST_F(STLWriterTest, EmptyMesh) {
    MeshPtr m1 = load_mesh("empty.stl");
    MeshPtr m2 = write_and_load("empty.stl", m1);

    assert_eq_vertices(m1, m2);
    assert_eq_faces(m1, m2);
}

TEST_F(STLWriterTest, Cube) {
    MeshPtr m1 = load_mesh("cube.stl");
    MeshPtr m2 = write_and_load("cube.stl", m1);

    // Because STL store mesh in triangle soup.  There is no guarentee that the
    // vertices/faces will be in the same order as the input mesh.
    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
}

TEST_F(STLWriterTest, 2D) {
    MeshPtr m1 = load_mesh("square_2D.obj");
    MeshPtr m2 = write_and_load("square_2D.stl", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
}

TEST_F(STLWriterTest, Ascii) {
    MeshPtr m1 = load_mesh("cube.stl");
    MeshPtr m2 = write_and_load("cube.stl", m1, true);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
}

