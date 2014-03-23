#pragma once

#include <string>
#include <set>

#include <Core/Exception.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

#include <MeshUtils/AttributeUtils.h>

class AttributeUtilsTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;
        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(MeshFactory()
                    .load_file(mesh_file)
                    .with_attribute("face_area")
                    .create());
        }

        void check_vertex_field(MeshPtr mesh) {
            using namespace AttributeUtils;
            size_t num_vertices = mesh->get_num_vertices();
            size_t num_faces = mesh->get_num_faces();
            size_t num_voxels = mesh->get_num_voxels();

            VectorF vertex_field = VectorF::Ones(num_vertices);

            VectorF result = convert_to_face_attribute(
                    *mesh.get(), vertex_field);
            ASSERT_FLOAT_EQ(num_faces, result.sum());

            result = convert_to_voxel_attribute(*mesh.get(), vertex_field);
            ASSERT_FLOAT_EQ(num_voxels, result.sum());
        }

        void check_face_field(MeshPtr mesh) {
            using namespace AttributeUtils;
            size_t num_vertices = mesh->get_num_vertices();
            size_t num_faces = mesh->get_num_faces();
            size_t num_voxels = mesh->get_num_voxels();

            VectorF face_field = VectorF::Ones(num_faces);

            VectorF result = convert_to_vertex_attribute(*mesh.get(), face_field);
            ASSERT_FLOAT_EQ(num_vertices, result.sum());

            ASSERT_THROW(convert_to_voxel_attribute(*mesh.get(), face_field),
                    NotImplementedError);
        }

        void check_voxel_field(MeshPtr mesh) {
            using namespace AttributeUtils;
            size_t num_vertices = mesh->get_num_vertices();
            size_t num_faces = mesh->get_num_faces();
            size_t num_voxels = mesh->get_num_voxels();

            VectorF voxel_field = VectorF::Ones(num_voxels);

            VectorF result = convert_to_vertex_attribute(*mesh.get(), voxel_field);
            ASSERT_FLOAT_EQ(num_vertices, result.sum());

            result = convert_to_face_attribute(*mesh.get(), voxel_field);
            ASSERT_FLOAT_EQ(num_faces, result.sum());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(AttributeUtilsTest, VertexField) {
    MeshPtr cube_tri = load_mesh("cube.obj");
    check_vertex_field(cube_tri);

    MeshPtr cube_tet = load_mesh("cube.msh");
    check_vertex_field(cube_tet);

    MeshPtr square = load_mesh("square_2D.obj");
    check_vertex_field(square);
}

TEST_F(AttributeUtilsTest, FaceField) {
    MeshPtr cube_tri = load_mesh("cube.obj");
    check_face_field(cube_tri);

    MeshPtr cube_tet = load_mesh("cube.msh");
    check_face_field(cube_tet);

    MeshPtr square = load_mesh("square_2D.obj");
    check_face_field(square);
}

TEST_F(AttributeUtilsTest, VoxelField) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    check_voxel_field(cube_tet);
}

