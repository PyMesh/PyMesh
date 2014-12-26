#pragma once
#include <string>
#include <memory>

#include <IO/MeshWriter.h>
#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

class OBJWriterTest : public ::testing::Test {
    protected:
        typedef std::shared_ptr<Mesh> MeshPtr;
        typedef std::shared_ptr<MeshWriter> WriterPtr;

        virtual void SetUp() {
            std::string proj_root =
                Environment::get_required("PYMESH_PATH");
            m_data_dir = proj_root + "/tests/data/";
            m_tmp_dir = "/tmp/";
        }

        MeshPtr load_mesh(const std::string& mesh_file) {
            MeshFactory factory;
            return MeshPtr(
                    factory.load_file(mesh_file).create());
        }

        void write_mesh(const std::string& mesh_file) {
            m_writer = WriterPtr(MeshWriter::create_writer(mesh_file));
        }

    protected:
        std::string m_data_dir;
        std::string m_tmp_dir;
        WriterPtr m_writer;
};

TEST_F(OBJWriterTest, WriteCube) {
    std::string mesh_file = m_data_dir + "cube.obj";
    std::string temp_file = m_tmp_dir + "cube.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write_mesh(*m1);
    MeshPtr m2 = load_mesh(temp_file);
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
    std::string mesh_file = m_data_dir + "tet.obj";
    std::string temp_file = m_tmp_dir + "tet.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write_mesh(*m1);
    MeshPtr m2 = load_mesh(temp_file);
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
    std::string mesh_file = m_data_dir + "square_2D.obj";
    std::string temp_file = m_tmp_dir + "square_2D.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write_mesh(*m1);
    MeshPtr m2 = load_mesh(temp_file);
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
    std::string mesh_file = m_data_dir + "cube.obj";
    std::string temp_file = m_tmp_dir + "cube_raw.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write(
            m1->get_vertices(),
            m1->get_faces(),
            m1->get_voxels(),
            m1->get_dim(),
            m1->get_vertex_per_face(),
            m1->get_vertex_per_voxel());
    MeshPtr m2 = load_mesh(temp_file);
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
    std::string mesh_file = m_data_dir + "tet.obj";
    std::string temp_file = m_tmp_dir + "tet_raw.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write(
            m1->get_vertices(),
            m1->get_faces(),
            m1->get_voxels(),
            m1->get_dim(),
            m1->get_vertex_per_face(),
            m1->get_vertex_per_voxel());
    MeshPtr m2 = load_mesh(temp_file);
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
    std::string mesh_file = m_data_dir + "square_2D.obj";
    std::string temp_file = m_tmp_dir + "square_2D_raw.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write(
            m1->get_vertices(),
            m1->get_faces(),
            m1->get_voxels(),
            m1->get_dim(),
            m1->get_vertex_per_face(),
            m1->get_vertex_per_voxel());
    MeshPtr m2 = load_mesh(temp_file);
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
    std::string mesh_file = m_data_dir + "cube.msh";
    std::string temp_file = m_tmp_dir + "cube1.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write_mesh(*m1);
    MeshPtr m2 = load_mesh(temp_file);
    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
}

TEST_F(OBJWriterTest, WriteHexToObj) {
    std::string mesh_file = m_data_dir + "hex.msh";
    std::string temp_file = m_tmp_dir + "hex1.obj";
    MeshPtr m1 = load_mesh(mesh_file);
    write_mesh(temp_file);
    m_writer->write_mesh(*m1);
    MeshPtr m2 = load_mesh(temp_file);
    ASSERT_EQ(m1->get_dim(), m2->get_dim());
    ASSERT_EQ(m1->get_vertex_per_face(), m2->get_vertex_per_face());
    ASSERT_EQ(m1->get_num_vertices(), m2->get_num_vertices());
    ASSERT_EQ(m1->get_num_faces(), m2->get_num_faces());
    ASSERT_EQ(m1->get_vertices(), m2->get_vertices());
    ASSERT_EQ(m1->get_faces(), m2->get_faces());
}

