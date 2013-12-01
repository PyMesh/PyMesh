#pragma once
#include <string>
#include <tr1/memory>

#include <Mesh.h>
#include <MeshFactory.h>
#include <Environment.h>

class MeshTest : public ::testing::Test {
    protected:
        typedef std::tr1::shared_ptr<Mesh> MeshPtr;
        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            std::string cube_tri_file   = project_dir + "/tests/data/cube.obj";
            std::string cube_tet_file   = project_dir + "/tests/data/cube.msh";
            std::string square_tri_file = project_dir + "/tests/data/square_2D.obj";
            m_cube_tri = MeshPtr(MeshFactory().load_file(cube_tri_file).create());
            m_cube_tet = MeshPtr(MeshFactory().load_file(cube_tet_file).create());
            m_square_tri = MeshPtr(MeshFactory().load_file(square_tri_file).create());
        }

    protected:
        MeshPtr m_cube_tri;
        MeshPtr m_cube_tet;
        MeshPtr m_square_tri;
};

TEST_F(MeshTest, Dim) {
    ASSERT_EQ(3, m_cube_tri->get_dim());
    ASSERT_EQ(3, m_cube_tet->get_dim());
    ASSERT_EQ(2, m_square_tri->get_dim());
}

TEST_F(MeshTest, NumVertices) {
    ASSERT_EQ(8, m_cube_tri->get_num_vertices());
    ASSERT_EQ(8, m_cube_tet->get_num_vertices());
    ASSERT_EQ(4, m_square_tri->get_num_vertices());
}

TEST_F(MeshTest, NumFaces) {
    ASSERT_EQ(12, m_cube_tri->get_num_faces());
    ASSERT_EQ(12, m_cube_tet->get_num_faces());
    ASSERT_EQ(2, m_square_tri->get_num_faces());
}

TEST_F(MeshTest, NumVoxels) {
    ASSERT_EQ(0, m_cube_tri->get_num_voxels());
    ASSERT_EQ(6, m_cube_tet->get_num_voxels());
    ASSERT_EQ(0, m_square_tri->get_num_voxels());
}

TEST_F(MeshTest, VertexPerFace) {
    ASSERT_EQ(3, m_cube_tri->get_vertex_per_face());
    ASSERT_EQ(3, m_cube_tet->get_vertex_per_face());
    ASSERT_EQ(3, m_square_tri->get_vertex_per_face());
}

TEST_F(MeshTest, VertexPerVoxel) {
    ASSERT_EQ(0, m_cube_tri->get_vertex_per_voxel());
    ASSERT_EQ(4, m_cube_tet->get_vertex_per_voxel());
    ASSERT_EQ(0, m_square_tri->get_vertex_per_voxel());
}

TEST_F(MeshTest, VertexSum) {
    // Since all test shapes are centered at the origin, the sum of all vertex
    // coordinates should be 0.
    ASSERT_DOUBLE_EQ(0.0, m_cube_tri->get_vertices().sum());
    ASSERT_DOUBLE_EQ(0.0, m_cube_tet->get_vertices().sum());
    ASSERT_DOUBLE_EQ(0.0, m_square_tri->get_vertices().sum());
}

TEST_F(MeshTest, FistVertex) {
    const VectorF& cube_tri_first = m_cube_tri->get_vertex(0);
    ASSERT_DOUBLE_EQ(-1.0, cube_tri_first[0]);
    ASSERT_DOUBLE_EQ(-1.0, cube_tri_first[1]);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_first[2]);
    const VectorF& cube_tet_first = m_cube_tet->get_vertex(0);
    ASSERT_DOUBLE_EQ(-1.0, cube_tri_first[0]);
    ASSERT_DOUBLE_EQ(-1.0, cube_tri_first[1]);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_first[2]);
    const VectorF& square_tri_first = m_square_tri->get_vertex(0);
    ASSERT_DOUBLE_EQ( 1.0, square_tri_first[0]);
    ASSERT_DOUBLE_EQ( 1.0, square_tri_first[1]);
}

TEST_F(MeshTest, LastVertex) {
    const VectorF& cube_tri_last = m_cube_tri->get_vertex(
            m_cube_tri->get_num_vertices() - 1);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_last[0]);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_last[1]);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_last[2]);
    const VectorF& cube_tet_last = m_cube_tet->get_vertex(
            m_cube_tet->get_num_vertices() - 1);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_last[0]);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_last[1]);
    ASSERT_DOUBLE_EQ( 1.0, cube_tri_last[2]);
    const VectorF& square_tri_last = m_square_tri->get_vertex(
            m_square_tri->get_num_vertices() - 1);
    ASSERT_DOUBLE_EQ(-1.0, square_tri_last[0]);
    ASSERT_DOUBLE_EQ(-1.0, square_tri_last[1]);
}

TEST_F(MeshTest, FirstFace) {
    const VectorI& cube_tri_first = m_cube_tri->get_face(0);
    ASSERT_EQ(5-1, cube_tri_first[0]);
    ASSERT_EQ(6-1, cube_tri_first[1]);
    ASSERT_EQ(2-1, cube_tri_first[2]);
    const VectorI& square_tri_first = m_square_tri->get_face(0);
    ASSERT_EQ(2-1, square_tri_first[0]);
    ASSERT_EQ(1-1, square_tri_first[1]);
    ASSERT_EQ(3-1, square_tri_first[2]);
}

TEST_F(MeshTest, LastFace) {
    const VectorI& cube_tri_last = m_cube_tri->get_face(
            m_cube_tri->get_num_faces() - 1);
    ASSERT_EQ(5-1, cube_tri_last[0]);
    ASSERT_EQ(8-1, cube_tri_last[1]);
    ASSERT_EQ(6-1, cube_tri_last[2]);
    const VectorI& square_tri_last = m_square_tri->get_face(
            m_square_tri->get_num_faces() - 1);
    ASSERT_EQ(4-1, square_tri_last[0]);
    ASSERT_EQ(2-1, square_tri_last[1]);
    ASSERT_EQ(3-1, square_tri_last[2]);
}

TEST_F(MeshTest, FirstVoxel) {
    const VectorI& cube_tet_first = m_cube_tet->get_voxel(0);
    ASSERT_EQ(2-1, cube_tet_first[0]);
    ASSERT_EQ(7-1, cube_tet_first[1]);
    ASSERT_EQ(1-1, cube_tet_first[2]);
    ASSERT_EQ(3-1, cube_tet_first[3]);
}

TEST_F(MeshTest, LastVoxel) {
    const VectorI& cube_tet_last = m_cube_tet->get_voxel(
            m_cube_tet->get_num_voxels() - 1);
    ASSERT_EQ(7-1, cube_tet_last[0]);
    ASSERT_EQ(5-1, cube_tet_last[1]);
    ASSERT_EQ(1-1, cube_tet_last[2]);
    ASSERT_EQ(8-1, cube_tet_last[3]);
}

//TEST_F(MeshTest, VertexAdjVertex) {
//    const VectorI& cube_tri_first = m_cube_tri->get_face(0);
//}



