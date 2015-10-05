/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>

#include <IO/MeshWriter.h>
#include "WriterTest.h"

class OBJWriterTest : public WriterTest {
};

TEST_F(OBJWriterTest, EmptyMesh) {
    MeshPtr m1 = load_mesh("empty.obj");
    MeshPtr m2 = write_and_load("empty.obj", m1);

    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_num_voxels(), m2->get_num_voxels());
}

TEST_F(OBJWriterTest, WriteCube) {
    MeshPtr m1 = load_mesh("cube.obj");
    MeshPtr m2 = write_and_load("cube.obj", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(OBJWriterTest, WriteTet) {
    MeshPtr m1 = load_mesh("tet.msh");
    MeshPtr m2 = write_and_load("tet.msh", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(OBJWriterTest, Write2DSquare) {
    MeshPtr m1 = load_mesh("square_2D.obj");
    MeshPtr m2 = write_and_load("square_2D.obj", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(OBJWriterTest, WriteCubeRaw) {
    MeshPtr m1 = load_mesh("square_2D.obj");
    MeshPtr m2 = write_and_load_raw("square_2D_raw.obj", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(OBJWriterTest, WriteTetRaw) {
    MeshPtr m1 = load_mesh("tet.obj");
    MeshPtr m2 = write_and_load_raw("tet_raw.obj", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(OBJWriterTest, Write2DSquareRaw) {
    MeshPtr m1 = load_mesh("square_2D.obj");
    MeshPtr m2 = write_and_load_raw("square_2D_raw.obj", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_vertex_per_voxel(), m2->get_vertex_per_voxel());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
    ASSERT_EQ(m1->get_voxels(), m2->get_voxels());
}

TEST_F(OBJWriterTest, ConvertMshToObj) {
    MeshPtr m1 = load_mesh("cube.msh");
    MeshPtr m2 = write_and_load("cube1.obj", m1);
    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
}

TEST_F(OBJWriterTest, WriteHexToObj) {
    MeshPtr m1 = load_mesh("hex.msh");
    MeshPtr m2 = write_and_load("hex1.obj", m1);

    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
}

