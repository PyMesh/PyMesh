#pragma once

#include <string>

#include <Misc/Environment.h>

class VertexLaplacianAttributeTest : public ::testing::Test {
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
                    .with_attribute("vertex_laplacian")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VertexLaplacianAttributeTest, ConvexShape) {
    MeshPtr cube = load_mesh("cube.msh");
    cube->add_attribute("vertex_normal");
    VectorF laplacian = cube->get_attribute("vertex_laplacian");
    VectorF normal = cube->get_attribute("vertex_normal");
    ASSERT_LT(0.0, laplacian.dot(normal));
}

TEST_F(VertexLaplacianAttributeTest, LaplacianSumCube) {
    MeshPtr cube = load_mesh("cube.msh");
    VectorF laplacian = cube->get_attribute("vertex_laplacian");
    ASSERT_FLOAT_EQ(0.0, laplacian.sum());
}

TEST_F(VertexLaplacianAttributeTest, LaplacianSumTet) {
    MeshPtr tet = load_mesh("tet.obj");
    VectorF laplacian = tet->get_attribute("vertex_laplacian");
    ASSERT_FLOAT_EQ(0.0, laplacian.sum());
}
