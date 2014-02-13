#pragma once

#include <string>

#include <Misc/Environment.h>

class VertexNormalAttributeTest : public ::testing::Test {
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
                    .with_attribute("vertex_normal")
                    .create());
        }

        // This test is only valid for convex shapes centered at the origin.
        void check_normals_point_away_from_center(MeshPtr mesh) {
            const size_t dim = mesh->get_dim();
            const size_t num_vertices = mesh->get_num_vertices();
            VectorF normals = mesh->get_attribute("vertex_normal");
            for (size_t i=0; i<num_vertices; i++) {
                VectorF n = normals.segment(i*dim, dim);
                VectorF x = mesh->get_vertex(i);
                ASSERT_GE(n.dot(x), 0.0);
            }
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VertexNormalAttributeTest, Creation) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_TRUE(square->has_attribute("vertex_normal"));
    ASSERT_EQ(square->get_num_vertices() * square->get_dim(),
            square->get_attribute("vertex_normal").size());

    MeshPtr cube_tet = load_mesh("cube.msh");
    ASSERT_TRUE(cube_tet->has_attribute("vertex_normal"));
    ASSERT_EQ(cube_tet->get_num_vertices() * cube_tet->get_dim(),
            cube_tet->get_attribute("vertex_normal").size());

    MeshPtr cube_tri = load_mesh("cube.obj");
    ASSERT_TRUE(cube_tri->has_attribute("vertex_normal"));
    ASSERT_EQ(cube_tri->get_num_vertices() * cube_tri->get_dim(),
            cube_tri->get_attribute("vertex_normal").size());
}

TEST_F(VertexNormalAttributeTest, Length) {
    MeshPtr square = load_mesh("square_2D.obj");
    VectorF square_normals = square->get_attribute("vertex_normal");
    ASSERT_FLOAT_EQ(4, square_normals.squaredNorm());

    MeshPtr cube_tet = load_mesh("cube.msh");
    VectorF cube_tet_normals = cube_tet->get_attribute("vertex_normal");
    ASSERT_FLOAT_EQ(8, cube_tet_normals.squaredNorm());

    MeshPtr cube_tri = load_mesh("cube.obj");
    VectorF cube_tri_normals = cube_tet->get_attribute("vertex_normal");
    ASSERT_FLOAT_EQ(8, cube_tri_normals.squaredNorm());
}

TEST_F(VertexNormalAttributeTest, AwayFromCenter) {
    MeshPtr square = load_mesh("square_2D.obj");
    check_normals_point_away_from_center(square);

    MeshPtr cube_tet = load_mesh("cube.msh");
    check_normals_point_away_from_center(cube_tet);

    MeshPtr cube_tri = load_mesh("cube.obj");
    check_normals_point_away_from_center(cube_tri);
}

