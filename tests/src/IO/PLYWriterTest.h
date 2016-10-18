/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <Mesh.h>
#include <IO/PLYWriter.h>

#include "WriterTest.h"

class PLYWriterTest : public WriterTest {
    protected:
};

TEST_F(PLYWriterTest, EmptyMesh) {
    MeshPtr m1 = load_mesh("empty.ply");
    MeshPtr m2 = write_and_load("empty.ply", m1);

    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_num_voxels(), m2->get_num_voxels());
}

TEST_F(PLYWriterTest, SurfaceMesh) {
    MeshPtr mesh = load_mesh("cube.ply");
    MeshPtr mesh2 = write_and_load("tmp_cube.ply", mesh);

    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);
}

TEST_F(PLYWriterTest, VolumeMesh) {
    MeshPtr mesh = load_mesh("cube.msh");
    MeshPtr mesh2 = write_and_load("tmp_cube_volume.ply", mesh);

    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);
}

TEST_F(PLYWriterTest, 2DMesh) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    MeshPtr mesh2 = write_and_load("tmp_square_2D.ply", mesh);

    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);
}

TEST_F(PLYWriterTest, VertexAttributes) {
    MeshPtr mesh = load_mesh("tet.msh");

    mesh->add_attribute("vertex_index");
    mesh->add_attribute("vertex_normal");

    std::string tmp_name = "tmp_tet_vertex_attr.ply";
    PLYWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("vertex_index");
    writer.with_attribute("vertex_normal");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "vertex_index");
    assert_eq_attribute(mesh, mesh2, "vertex_normal");
}

TEST_F(PLYWriterTest, FaceAttributes) {
    MeshPtr mesh = load_mesh("cube.obj");

    mesh->add_attribute("face_index");
    mesh->add_attribute("face_normal");

    std::string tmp_name = "tmp_cube_face_attr.ply";
    PLYWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("face_index");
    writer.with_attribute("face_normal");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "face_index");
    assert_eq_attribute(mesh, mesh2, "face_normal");
}

TEST_F(PLYWriterTest, VoxelAttributes) {
    MeshPtr mesh = load_mesh("cube.msh");
    const size_t dim = mesh->get_dim();
    const size_t num_voxels = mesh->get_num_voxels();

    VectorF vector_field = VectorF::LinSpaced(num_voxels * dim, 0.0, 1.0);
    VectorF tensor_field = VectorF::LinSpaced(
            num_voxels * dim * (dim + 1) / 2, 0.0, 1.0);

    mesh->add_attribute("voxel_index");
    mesh->add_attribute("voxel_vector");
    mesh->set_attribute("voxel_vector", vector_field);
    mesh->add_attribute("voxel_tensor");
    mesh->set_attribute("voxel_tensor", tensor_field);

    std::string tmp_name = "tmp_cube_voxel_attr.ply";
    PLYWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("voxel_index");
    writer.with_attribute("voxel_vector");
    writer.with_attribute("voxel_tensor");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "voxel_index");
    assert_eq_attribute(mesh, mesh2, "voxel_vector");
    assert_eq_voxel_tensor_attribute(mesh, mesh2, "voxel_tensor");
}

TEST_F(PLYWriterTest, DuplicatedAttributes) {
    MeshPtr mesh = load_mesh("cube.msh");
    VectorF vertex_field = VectorF::Zero(mesh->get_num_vertices());
    VectorF face_field = VectorF::Zero(mesh->get_num_faces());
    mesh->add_attribute("vertex_red");
    mesh->set_attribute("vertex_red", vertex_field);
    mesh->add_attribute("face_red");
    mesh->set_attribute("face_red", face_field);

    std::string tmp_name = "tmp_cube_voxel_attr.ply";
    PLYWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("vertex_red");
    writer.with_attribute("face_red");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "vertex_red");
    assert_eq_attribute(mesh, mesh2, "face_red");
}

