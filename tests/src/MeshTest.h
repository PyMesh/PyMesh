/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <Mesh.h>
#include <TestBase.h>

using ::testing::Contains;

class MeshTest : public TestBase {
    protected:
        typedef Mesh::Ptr MeshPtr;
        virtual void SetUp() {
            TestBase::SetUp();
            m_cube_tri = load_mesh("cube.obj");
            m_cube_tet = load_mesh("cube.msh");
            m_square_tri = load_mesh("square_2D.obj");
            m_cube_hex = load_mesh("hex.msh");
            m_quad = load_mesh("quad.obj");
        }

    protected:
        template <typename T>
        std::vector<T> to_vector(size_t size, const T* data) {
            return std::vector<T>(data, data+size);
        }

        void check_vertex_in_face_are_adjacent(const MeshPtr& mesh);
        void check_vertex_in_voxel_are_adjacent(const MeshPtr& mesh);
        void check_vertex_face_adjacency(const MeshPtr& mesh);
        void check_vertex_voxel_adjacency(const MeshPtr& mesh);
        void check_vertex_vertex_adjacency_is_symmetric(const MeshPtr& mesh);
        void check_face_face_adjacency_is_symmetric(const MeshPtr& mesh);
        void check_voxel_voxel_adjacency_is_symmetric(const MeshPtr& mesh);
        void check_face_voxel_adjacency(const MeshPtr& mesh);
        void check_voxel_face_adjacency(const MeshPtr& mesh);

    protected:
        MeshPtr m_cube_tri;
        MeshPtr m_cube_tet;
        MeshPtr m_square_tri;
        MeshPtr m_cube_hex;
        MeshPtr m_quad;
};
#include "MeshTest.inl"

TEST_F(MeshTest, Dim) {
    ASSERT_EQ(3, m_cube_tri->get_dim());
    ASSERT_EQ(3, m_cube_tet->get_dim());
    ASSERT_EQ(2, m_square_tri->get_dim());
    ASSERT_EQ(3, m_cube_hex->get_dim());
}

TEST_F(MeshTest, NumVertices) {
    ASSERT_EQ(8, m_cube_tri->get_num_vertices());
    ASSERT_EQ(8, m_cube_tet->get_num_vertices());
    ASSERT_EQ(4, m_square_tri->get_num_vertices());
    ASSERT_EQ(8, m_cube_hex->get_num_vertices());
}

TEST_F(MeshTest, NumFaces) {
    ASSERT_EQ(12, m_cube_tri->get_num_faces());
    ASSERT_EQ(12, m_cube_tet->get_num_faces());
    ASSERT_EQ(2, m_square_tri->get_num_faces());
    ASSERT_EQ(6, m_cube_hex->get_num_faces());
}

TEST_F(MeshTest, NumVoxels) {
    ASSERT_EQ(0, m_cube_tri->get_num_voxels());
    ASSERT_EQ(6, m_cube_tet->get_num_voxels());
    ASSERT_EQ(0, m_square_tri->get_num_voxels());
    ASSERT_EQ(1, m_cube_hex->get_num_voxels());
}

TEST_F(MeshTest, VertexPerFace) {
    ASSERT_EQ(3, m_cube_tri->get_vertex_per_face());
    ASSERT_EQ(3, m_cube_tet->get_vertex_per_face());
    ASSERT_EQ(3, m_square_tri->get_vertex_per_face());
    ASSERT_EQ(4, m_cube_hex->get_vertex_per_face());
}

TEST_F(MeshTest, VertexPerVoxel) {
    ASSERT_EQ(0, m_cube_tri->get_vertex_per_voxel());
    ASSERT_EQ(4, m_cube_tet->get_vertex_per_voxel());
    ASSERT_EQ(0, m_square_tri->get_vertex_per_voxel());
    ASSERT_EQ(8, m_cube_hex->get_vertex_per_voxel());
}

TEST_F(MeshTest, VertexSum) {
    // Since all test shapes are centered at the origin, the sum of all vertex
    // coordinates should be 0.
    ASSERT_DOUBLE_EQ(0.0, m_cube_tri->get_vertices().sum());
    ASSERT_DOUBLE_EQ(0.0, m_cube_tet->get_vertices().sum());
    ASSERT_DOUBLE_EQ(0.0, m_square_tri->get_vertices().sum());
    ASSERT_DOUBLE_EQ(0.0, m_cube_hex->get_vertices().sum());
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
    const VectorF& cube_hex_first = m_cube_hex->get_vertex(0);
    ASSERT_DOUBLE_EQ(-1.0, cube_hex_first[0]);
    ASSERT_DOUBLE_EQ(-1.0, cube_hex_first[1]);
    ASSERT_DOUBLE_EQ(-1.0, cube_hex_first[2]);
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
    const VectorF& cube_hex_last = m_cube_hex->get_vertex(
            m_cube_hex->get_num_vertices() - 1);
    ASSERT_DOUBLE_EQ( 1.0, cube_hex_last[0]);
    ASSERT_DOUBLE_EQ( 1.0, cube_hex_last[1]);
    ASSERT_DOUBLE_EQ( 1.0, cube_hex_last[2]);
}

TEST_F(MeshTest, FirstFace) {
    const VectorI& cube_tri_first = m_cube_tri->get_face(0);
    ASSERT_EQ(5-1, cube_tri_first[0]);
    ASSERT_EQ(6-1, cube_tri_first[1]);
    ASSERT_EQ(2-1, cube_tri_first[2]);
    const VectorI& square_tri_first = m_square_tri->get_face(0);
    ASSERT_EQ(1-1, square_tri_first[0]);
    ASSERT_EQ(2-1, square_tri_first[1]);
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
    ASSERT_EQ(3-1, square_tri_last[1]);
    ASSERT_EQ(2-1, square_tri_last[2]);
}

TEST_F(MeshTest, FirstVoxel) {
    const VectorI& cube_tet_first = m_cube_tet->get_voxel(0);
    ASSERT_EQ(2-1, cube_tet_first[0]);
    ASSERT_EQ(7-1, cube_tet_first[1]);
    ASSERT_EQ(1-1, cube_tet_first[2]);
    ASSERT_EQ(3-1, cube_tet_first[3]);
    const VectorI& cube_hex_first = m_cube_hex->get_voxel(0);
    ASSERT_EQ(1-1, cube_hex_first[0]);
    ASSERT_EQ(5-1, cube_hex_first[1]);
    ASSERT_EQ(6-1, cube_hex_first[2]);
    ASSERT_EQ(2-1, cube_hex_first[3]);
    ASSERT_EQ(3-1, cube_hex_first[4]);
    ASSERT_EQ(7-1, cube_hex_first[5]);
    ASSERT_EQ(8-1, cube_hex_first[6]);
    ASSERT_EQ(4-1, cube_hex_first[7]);
}

TEST_F(MeshTest, LastVoxel) {
    const VectorI& cube_tet_last = m_cube_tet->get_voxel(
            m_cube_tet->get_num_voxels() - 1);
    ASSERT_EQ(7-1, cube_tet_last[0]);
    ASSERT_EQ(5-1, cube_tet_last[1]);
    ASSERT_EQ(1-1, cube_tet_last[2]);
    ASSERT_EQ(8-1, cube_tet_last[3]);
}

TEST_F(MeshTest, VertexAdjVertex) {
    check_vertex_in_face_are_adjacent(m_cube_tri);
    check_vertex_in_face_are_adjacent(m_cube_tet);
    check_vertex_in_face_are_adjacent(m_square_tri);
    check_vertex_in_voxel_are_adjacent(m_cube_tri);
    check_vertex_in_voxel_are_adjacent(m_cube_tet);
    check_vertex_in_voxel_are_adjacent(m_square_tri);
}

TEST_F(MeshTest, VertexAdjFace) {
    check_vertex_face_adjacency(m_cube_tri);
    check_vertex_face_adjacency(m_cube_tet);
    check_vertex_face_adjacency(m_square_tri);
}

TEST_F(MeshTest, VertexAdjVoxel) {
    check_vertex_voxel_adjacency(m_cube_tri);
    check_vertex_voxel_adjacency(m_cube_tet);
    check_vertex_voxel_adjacency(m_square_tri);
}

TEST_F(MeshTest, FaceAdjFaceSymmetric) {
    check_face_face_adjacency_is_symmetric(m_cube_tri);
    check_face_face_adjacency_is_symmetric(m_cube_tet);
    check_face_face_adjacency_is_symmetric(m_square_tri);
}

TEST_F(MeshTest, VoxelAdjVoxelSymmetric) {
    check_voxel_voxel_adjacency_is_symmetric(m_cube_tri);
    check_voxel_voxel_adjacency_is_symmetric(m_cube_tet);
    check_voxel_voxel_adjacency_is_symmetric(m_square_tri);
}

TEST_F(MeshTest, FaceAdjVoxel) {
    check_face_voxel_adjacency(m_cube_tri);
    check_face_voxel_adjacency(m_cube_tet);
    check_face_voxel_adjacency(m_square_tri);
}

TEST_F(MeshTest, VoxelAdjFace) {
    check_voxel_face_adjacency(m_cube_tri);
    check_voxel_face_adjacency(m_cube_tet);
    check_voxel_face_adjacency(m_square_tri);
}

TEST_F(MeshTest, QuadAdj) {
    m_quad->enable_connectivity();
    ASSERT_EQ(2, m_quad->get_vertex_adjacent_vertices(0).size());
    ASSERT_EQ(2, m_quad->get_vertex_adjacent_vertices(1).size());
    ASSERT_EQ(2, m_quad->get_vertex_adjacent_vertices(2).size());
    ASSERT_EQ(2, m_quad->get_vertex_adjacent_vertices(3).size());

    check_vertex_vertex_adjacency_is_symmetric(m_quad);
}

TEST_F(MeshTest, HexAdj) {
    m_cube_hex->enable_connectivity();
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(0).size());
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(1).size());
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(2).size());
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(3).size());
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(4).size());
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(5).size());
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(6).size());
    ASSERT_EQ(3, m_cube_hex->get_vertex_adjacent_vertices(7).size());

    check_face_face_adjacency_is_symmetric(m_cube_hex);
    check_vertex_vertex_adjacency_is_symmetric(m_cube_hex);
}

