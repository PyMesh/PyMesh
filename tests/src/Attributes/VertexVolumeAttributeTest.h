#pragma once

#include <string>

#include <Misc/Environment.h>

class VertexVolumeAttributeTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string proj_root =
                Environment::get_required("PYMESH_PATH");
            m_data_dir = proj_root + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .with_attribute("vertex_volume")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VertexVolumeAttributeTest, tet) {
    MeshPtr tet = load_mesh("tet.msh");
    ASSERT_TRUE(tet->has_attribute("vertex_volume"));
    VectorF volumes = tet->get_attribute("vertex_volume");
    ASSERT_TRUE((volumes.array() == volumes[0]).all());
}

TEST_F(VertexVolumeAttributeTest, hex) {
    MeshPtr hex = load_mesh("hex.msh");
    ASSERT_TRUE(hex->has_attribute("vertex_volume"));
    VectorF volumes = hex->get_attribute("vertex_volume");
    ASSERT_FLOAT_EQ(1.0, volumes[0]);
    ASSERT_TRUE((volumes.array() == volumes[0]).all());
}

TEST_F(VertexVolumeAttributeTest, surface_mesh) {
    MeshPtr mesh = load_mesh("cube.obj");
    ASSERT_TRUE(mesh->has_attribute("vertex_volume"));
    VectorF volumes = mesh->get_attribute("vertex_volume");
    ASSERT_EQ(0, volumes.size());
}

TEST_F(VertexVolumeAttributeTest, 2D_mesh) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    ASSERT_TRUE(mesh->has_attribute("vertex_volume"));
    VectorF volumes = mesh->get_attribute("vertex_volume");
    ASSERT_EQ(0, volumes.size());
}
