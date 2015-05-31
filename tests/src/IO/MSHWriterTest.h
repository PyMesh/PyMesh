/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <Mesh.h>
#include <IO/MSHWriter.h>

#include "WriterTest.h"

class MSHWriterTest : public WriterTest {
};

TEST_F(MSHWriterTest, EmptyMesh) {
    MatrixFr vertices = MatrixFr::Zero(0, 3);
    MatrixIr faces    = MatrixIr::Zero(0, 3);
    MatrixIr voxels   = MatrixIr::Zero(0, 4);
    MeshPtr mesh = load_data(vertices, faces, voxels);

    std::string tmp_name = "empty.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);

    remove(tmp_name);
}

TEST_F(MSHWriterTest, SurfaceMesh) {
    std::string filename = "cube.obj";
    MeshPtr mesh = load_mesh(filename);

    std::string tmp_name = "tmp_cube_surface.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);

    remove(tmp_name);
}

TEST_F(MSHWriterTest, VolumeMesh) {
    std::string filename = "cube.msh";
    MeshPtr mesh = load_mesh(filename);

    std::string tmp_name = "tmp_cube_volume.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);

    remove(tmp_name);
}

TEST_F(MSHWriterTest, HexMesh) {
    std::string filename = "hex.msh";
    MeshPtr mesh = load_mesh(filename);

    std::string tmp_name = "tmp_hex.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);

    remove(tmp_name);
}

TEST_F(MSHWriterTest, QuadMesh) {
    std::string filename = "quad.obj";
    MeshPtr mesh = load_mesh(filename);

    std::string tmp_name = "tmp_quad.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);

    remove(tmp_name);
}

TEST_F(MSHWriterTest, VertexAttributes) {
    std::string filename = "tet.msh";
    MeshPtr mesh = load_mesh(filename);

    mesh->add_attribute("vertex_index");
    mesh->add_attribute("vertex_normal");

    std::string tmp_name = "tmp_tet_vertex_attr.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("vertex_index");
    writer.with_attribute("vertex_normal");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "vertex_index");
    assert_eq_attribute(mesh, mesh2, "vertex_normal");

    remove(tmp_name);
}

TEST_F(MSHWriterTest, FaceAttributes) {
    std::string filename = "cube.obj";
    MeshPtr mesh = load_mesh(filename);

    mesh->add_attribute("face_index");
    mesh->add_attribute("face_normal");

    std::string tmp_name = "tmp_cube_face_attr.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("face_index");
    writer.with_attribute("face_normal");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "face_index");
    assert_eq_attribute(mesh, mesh2, "face_normal");

    remove(tmp_name);
}

TEST_F(MSHWriterTest, VoxelAttributes) {
    std::string filename = "cube.msh";
    MeshPtr mesh = load_mesh(filename);
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

    std::string tmp_name = "tmp_cube_voxel_attr.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("voxel_index");
    writer.with_attribute("voxel_vector");
    writer.with_attribute("voxel_tensor");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "voxel_index");
    assert_eq_attribute(mesh, mesh2, "voxel_vector");
    assert_eq_voxel_tensor_attribute(mesh, mesh2, "voxel_tensor");

    remove(tmp_name);
}

