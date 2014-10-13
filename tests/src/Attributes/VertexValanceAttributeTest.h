#pragma once

#include <Misc/Environment.h>

class VertexValanceAttributeTest : public ::testing::Test {
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
                    .with_attribute("vertex_valance")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VertexValanceAttributeTest, Quad) {
    MeshPtr quad = load_mesh("quad.obj");
    ASSERT_TRUE(quad->has_attribute("vertex_valance"));
    VectorF valance = quad->get_attribute("vertex_valance");
    ASSERT_EQ(4, valance.size());
    ASSERT_FLOAT_EQ(2.0, valance.minCoeff());
    ASSERT_FLOAT_EQ(2.0, valance.maxCoeff());
}

TEST_F(VertexValanceAttributeTest, Hex) {
    MeshPtr quad = load_mesh("hex.msh");
    ASSERT_TRUE(quad->has_attribute("vertex_valance"));
    VectorF valance = quad->get_attribute("vertex_valance");
    ASSERT_EQ(8, valance.size());
    ASSERT_FLOAT_EQ(3.0, valance.minCoeff());
    ASSERT_FLOAT_EQ(3.0, valance.maxCoeff());
}

TEST_F(VertexValanceAttributeTest, Tet) {
    MeshPtr quad = load_mesh("tet.msh");
    ASSERT_TRUE(quad->has_attribute("vertex_valance"));
    VectorF valance = quad->get_attribute("vertex_valance");
    ASSERT_EQ(4, valance.size());
    ASSERT_FLOAT_EQ(3.0, valance.minCoeff());
    ASSERT_FLOAT_EQ(3.0, valance.maxCoeff());
}
