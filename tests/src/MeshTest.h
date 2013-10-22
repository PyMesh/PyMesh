#pragma once
#include <string>

#include <Mesh.h>
#include <MeshFactory.h>
#include <Environment.h>

class MeshTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            std::string mesh_file = project_dir + "/tests/data/cube.obj";
            m_cube = MeshFactory().load_file(mesh_file).create();
        }

        virtual void TearDown() {
            delete m_cube;
        }

    protected:
        Mesh* m_cube;
};

TEST_F(MeshTest, dim_test) {
    ASSERT_EQ(3, m_cube->get_dim());
}
