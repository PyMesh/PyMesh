/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>

#include <Mesh.h>
#include <MeshFactory.h>

#include <TestBase.h>

using ::testing::Contains;

class MeshFactoryTest : public TestBase {
    protected:
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
};

TEST_F(MeshFactoryTest, NonExistFile) {
    std::string non_exists_obj_name = "non_exist.obj";
    std::string non_exists_stl_name = "non_exist.stl";
    std::string non_exists_msh_name = "non_exist.msh";

    ASSERT_THROW(load_mesh(non_exists_obj_name), IOError);
    ASSERT_THROW(load_mesh(non_exists_stl_name), IOError);
    ASSERT_THROW(load_mesh(non_exists_msh_name), IOError);
}

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

