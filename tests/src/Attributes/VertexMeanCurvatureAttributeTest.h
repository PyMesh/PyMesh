#pragma once

#include <string>

#include <Misc/Environment.h>

class VertexMeanCurvatureAttributeTest : public ::testing::Test {
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
                    .with_attribute("vertex_mean_curvature")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VertexMeanCurvatureAttributeTest, ConvexShape) {
    MeshPtr cube = load_mesh("cube.msh");
    const size_t num_vertices = cube->get_num_vertices();
    VectorF mean_curvature = cube->get_attribute("vertex_mean_curvature");
    ASSERT_EQ(num_vertices, mean_curvature.size());
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_LE(0.0, mean_curvature[i]);
    }
}
