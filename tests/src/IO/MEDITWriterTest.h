/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>

#include <IO/MeshWriter.h>
#include "WriterTest.h"

class MEDITWriterTest : public WriterTest {
};

TEST_F(MEDITWriterTest, EmptyMesh) {
    MeshPtr m1 = load_mesh("empty.mesh");
    MeshPtr m2 = write_and_load("empty.mesh", m1);

    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_num_voxels(), m2->get_num_voxels());
}

TEST_F(MEDITWriterTest, WriteCube) {
    MeshPtr m1 = load_mesh("cube.obj");
    MeshPtr m2 = write_and_load("cube.mesh", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(MEDITWriterTest, WriteTet) {
    MeshPtr m1 = load_mesh("tet.mesh");
    MeshPtr m2 = write_and_load("tet.mesh", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(MEDITWriterTest, WriteHex) {
    MeshPtr m1 = load_mesh("hex.msh");
    MeshPtr m2 = write_and_load("hex.mesh", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(MEDITWriterTest, 2D) {
    MeshPtr m1 = load_mesh("square_2D.obj");
    MeshPtr m2 = write_and_load("square_2D.mesh", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}
