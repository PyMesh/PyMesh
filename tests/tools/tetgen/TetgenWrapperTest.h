#pragma once
#include <tetgen/TetgenWrapper.h>

#include <string>

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

class TetgenWrapperTest : public ::testing::Test {
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

        MatrixFr extract_vertices(MeshPtr mesh) {
            const size_t num_vertices = mesh->get_num_vertices();
            const size_t dim = mesh->get_dim();
            VectorF vertices = mesh->get_vertices();
            MatrixFr result(num_vertices, dim);
            std::copy(vertices.data(), vertices.data() + num_vertices * dim,
                    result.data());
            return result;
        }

        MatrixIr extract_faces(MeshPtr mesh) {
            const size_t num_faces = mesh->get_num_faces();
            const size_t vertex_per_face = mesh->get_vertex_per_face();
            VectorI faces = mesh->get_faces();
            MatrixIr result(num_faces, vertex_per_face);
            std::copy(faces.data(), faces.data() + num_faces * vertex_per_face,
                    result.data());
            return result;
        }

    protected:
        std::string m_data_dir;
};

TEST_F(TetgenWrapperTest, Cube) {
    MeshPtr mesh = load_mesh("cube.obj");

    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    TetgenWrapper wrapper(vertices, faces);
    wrapper.run("qpYQ");

    MatrixFr tet_vertices = wrapper.get_vertices();
    MatrixIr tet_faces = wrapper.get_faces();
    MatrixIr tet_voxels = wrapper.get_voxels();

    ASSERT_GE(tet_vertices.rows(), vertices.rows());
    ASSERT_GT(tet_faces.rows(), 0);
    ASSERT_GT(tet_voxels.rows(), 0);
}
