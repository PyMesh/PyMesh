#pragma once

#include <Core/Exception.h>
#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <IO/MeshWriter.h>

#include <Envelope/VoxelGrid.h>

class VoxelGridTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .create());
        }

        void save_mesh(const std::string& filename, MeshPtr mesh) {
            MeshWriter* writer = MeshWriter::create_writer(filename);
            writer->write_mesh(*mesh);
            delete writer;
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VoxelGridTest, creation_2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    VoxelGrid<2> grid(0.5);
    grid.insert_mesh(mesh);
    grid.create_grid();

    const size_t num_vertices = mesh->get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        VectorF p = mesh->get_vertex(i);
        ASSERT_TRUE(grid.lookup(p) > 0);
    }
    ASSERT_TRUE(grid.lookup(Vector2F::Zero()) > 0);
}

TEST_F(VoxelGridTest, creation_3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    VoxelGrid<3> grid(0.5);
    grid.insert_mesh(mesh);
    grid.create_grid();

    const size_t num_vertices = mesh->get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        VectorF p = mesh->get_vertex(i);
        ASSERT_TRUE(grid.lookup(p) > 0);
    }
    ASSERT_TRUE(grid.lookup(Vector3F::Zero()) > 0);
}

TEST_F(VoxelGridTest, exterior_2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    const Float cell_size = 0.5;
    VoxelGrid<2> grid(cell_size);
    grid.insert_mesh(mesh);
    grid.create_grid();

    mesh->add_attribute("vertex_normal");
    VectorF normals = mesh->get_attribute("vertex_normal");

    const size_t num_vertices = mesh->get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        const Vector2F& n = normals.segment<2>(i*2);
        Vector2F p = mesh->get_vertex(i) + n * cell_size;
        ASSERT_EQ(0, grid.lookup(p));
    }
}

TEST_F(VoxelGridTest, exterior_3D) {
    MeshPtr mesh = load_mesh("cube.obj");
    const Float cell_size = 0.5;
    VoxelGrid<3> grid(cell_size);
    grid.insert_mesh(mesh);
    grid.create_grid();

    mesh->add_attribute("vertex_normal");
    VectorF normals = mesh->get_attribute("vertex_normal");

    const size_t num_vertices = mesh->get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        const Vector3F& n = normals.segment<3>(i*3);
        Vector3F p = mesh->get_vertex(i) + n * cell_size;
        ASSERT_EQ(0, grid.lookup(p));
    }
}

TEST_F(VoxelGridTest, VoxelMesh_2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    const Float cell_size = 0.5;
    VoxelGrid<2> grid(cell_size);
    grid.insert_mesh(mesh);
    grid.create_grid();
    MeshPtr quad_mesh = grid.get_voxel_mesh();

    size_t num_occupied_cells = 0;
    for (auto itr = grid.begin(); itr != grid.end(); itr++) {
        if (*itr != 0) num_occupied_cells++;
    }

    ASSERT_EQ(2, quad_mesh->get_dim());
    ASSERT_EQ(4, quad_mesh->get_vertex_per_face());
    ASSERT_EQ(num_occupied_cells, quad_mesh->get_num_faces());
    save_mesh("tmp_quad.msh", quad_mesh);
}

TEST_F(VoxelGridTest, VoxelMesh_3D) {
    MeshPtr mesh = load_mesh("cube.obj");
    const Float cell_size = 0.5;
    VoxelGrid<3> grid(cell_size);
    grid.insert_mesh(mesh);
    grid.create_grid();
    MeshPtr hex_mesh = grid.get_voxel_mesh();

    size_t num_occupied_cells = 0;
    for (auto itr = grid.begin(); itr != grid.end(); itr++) {
        if (*itr != 0) num_occupied_cells++;
    }

    ASSERT_EQ(3, hex_mesh->get_dim());
    ASSERT_EQ(8, hex_mesh->get_vertex_per_voxel());
    ASSERT_EQ(num_occupied_cells, hex_mesh->get_num_voxels());

    save_mesh("tmp_cube.msh", hex_mesh);
}

