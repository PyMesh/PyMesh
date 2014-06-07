#pragma once

#include <string>

#include <Misc/Environment.h>

class VertexIndexAttributeTest : public ::testing::Test {
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
                    .with_attribute("vertex_index")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VertexIndexAttributeTest, cube) {
    MeshPtr cube = load_mesh("cube.msh");
    const size_t num_vertices = cube->get_num_vertices();
    const VectorF& indices = cube->get_attribute("vertex_index");
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_FLOAT_EQ(Float(i), indices[i]);
    }
}
