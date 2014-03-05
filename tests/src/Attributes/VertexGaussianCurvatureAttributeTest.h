#pragma once

#include <string>

#include <Misc/Environment.h>

class VertexGaussianCurvatureAttributeTest : public ::testing::Test {
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
                    .with_attribute("vertex_gaussian_curvature")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VertexGaussianCurvatureAttributeTest, CubeTest) {
    MeshPtr mesh = load_mesh("cube.obj");
    const size_t num_vertices = mesh->get_num_vertices();
    VectorF gaussian_curvature = mesh->get_attribute("vertex_gaussian_curvature");

    ASSERT_EQ(num_vertices, gaussian_curvature.size());
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_FLOAT_EQ(M_PI*0.5, gaussian_curvature[i]);
    }
}

TEST_F(VertexGaussianCurvatureAttributeTest, TetTest) {
    MeshPtr mesh = load_mesh("tet.obj");
    const size_t num_vertices = mesh->get_num_vertices();
    VectorF gaussian_curvature = mesh->get_attribute("vertex_gaussian_curvature");

    ASSERT_EQ(num_vertices, gaussian_curvature.size());
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_NEAR(M_PI, gaussian_curvature[i], 1e-5);
    }
}
