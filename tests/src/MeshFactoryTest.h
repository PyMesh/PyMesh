#pragma once
#include <string>

#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

using ::testing::Contains;

class MeshFactoryTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string proj_root = Environment::get("PYMESH_PATH");
            m_data_dir = proj_root + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string full_path = m_data_dir + filename;
            return MeshPtr(MeshFactory().load_file(full_path).create());
        }

        MeshPtr load_data(VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
            return MeshPtr(MeshFactory().load_data(
                        vertices, faces, voxels,
                        dim, vertex_per_face, vertex_per_voxel).create());
        }

        MeshPtr copy_mesh_data(const MeshPtr mesh) {
            return load_data(mesh->get_vertices(), mesh->get_faces(), mesh->get_voxels(),
                    mesh->get_dim(),
                    mesh->get_vertex_per_face(),
                    mesh->get_vertex_per_voxel());
        }

        void ASSERT_MESH_EQ(const MeshPtr mesh1, const MeshPtr mesh2) const {
            ASSERT_EQ(mesh1->get_dim(), mesh2->get_dim());
            ASSERT_EQ(mesh1->get_vertices(), mesh2->get_vertices());
            ASSERT_EQ(mesh1->get_faces(), mesh2->get_faces());
            ASSERT_EQ(mesh1->get_voxels(), mesh2->get_voxels());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(MeshFactoryTest, LoadFile) {
    std::string cube_tri_name = "cube.obj";
    std::string cube_tet_name = "cube.msh";
    std::string cube_hex_name = "hex.msh";
    std::string square_name   = "square_2D.obj";

    MeshPtr cube_tri = load_mesh(cube_tri_name);
    MeshPtr cube_tet = load_mesh(cube_tet_name);
    MeshPtr cube_hex = load_mesh(cube_hex_name);
    MeshPtr square   = load_mesh(square_name);

    ASSERT_EQ(3, cube_tri->get_dim());
    ASSERT_EQ(3, cube_tet->get_dim());
    ASSERT_EQ(3, cube_hex->get_dim());
    ASSERT_EQ(2, square  ->get_dim());

    ASSERT_EQ(8, cube_tri->get_num_vertices());
    ASSERT_EQ(8, cube_tet->get_num_vertices());
    ASSERT_EQ(8, cube_hex->get_num_vertices());
    ASSERT_EQ(4, square  ->get_num_vertices());
}

TEST_F(MeshFactoryTest, LoadData) {
    std::string cube_tri_name = "cube.obj";
    std::string cube_tet_name = "cube.msh";
    std::string cube_hex_name = "hex.msh";
    std::string square_name   = "square_2D.obj";

    MeshPtr cube_tri = load_mesh(cube_tri_name);
    MeshPtr cube_tet = load_mesh(cube_tet_name);
    MeshPtr cube_hex = load_mesh(cube_hex_name);
    MeshPtr square   = load_mesh(square_name);

    MeshPtr cube_tri_cp = copy_mesh_data(cube_tri);
    MeshPtr cube_tet_cp = copy_mesh_data(cube_tet);
    MeshPtr cube_hex_cp = copy_mesh_data(cube_hex);
    MeshPtr square_cp   = copy_mesh_data(square);

    ASSERT_MESH_EQ(cube_tri, cube_tri_cp);
    ASSERT_MESH_EQ(cube_tet, cube_tet_cp);
    ASSERT_MESH_EQ(cube_hex, cube_hex_cp);
    ASSERT_MESH_EQ(square  , square_cp);
}

