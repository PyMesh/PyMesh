#pragma once

#include <string>
#include <iostream>

#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

#include <MeshUtils/IsolatedVertexRemoval.h>

class IsolatedVertexRemovalTest : public ::testing::Test {
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
                    .create());
        }

        void assert_matrix_eq(const MatrixFr& m1, const MatrixFr& m2) {
            MatrixFr diff = m1 - m2;
            ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
            ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
        }

        void assert_matrix_eq(const MatrixIr& m1, const MatrixIr& m2) {
            MatrixIr diff = m1 - m2;
            ASSERT_EQ(0, diff.minCoeff());
            ASSERT_EQ(0, diff.maxCoeff());
        }

        void add_extra_vertex_and_check(MeshPtr mesh) {
            const size_t dim = mesh->get_dim();
            const size_t num_vertices = mesh->get_num_vertices();
            const size_t num_faces = mesh->get_num_faces();
            const size_t vertex_per_face = mesh->get_vertex_per_face();

            MatrixFr vertices = Eigen::Map<MatrixFr>(
                    mesh->get_vertices().data(), num_vertices, dim);
            MatrixIr faces = Eigen::Map<MatrixIr>(
                    mesh->get_faces().data(), num_faces, vertex_per_face);

            MatrixFr modified_vertices(num_vertices+1, dim);
            modified_vertices.block(1,0, num_vertices, dim) = vertices;
            modified_vertices.row(0) = VectorF::Zero(dim);
            MatrixIr modified_faces = faces + MatrixIr::Ones(num_faces, vertex_per_face);

            IsolatedVertexRemoval remover(modified_vertices, modified_faces);
            remover.run();

            MatrixFr result_vertices = remover.get_vertices();
            MatrixIr result_faces = remover.get_faces();

            assert_matrix_eq(vertices, result_vertices);
            assert_matrix_eq(faces, result_faces);
        }

    protected:
        std::string m_data_dir;
};

TEST_F(IsolatedVertexRemovalTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    add_extra_vertex_and_check(mesh);
}

TEST_F(IsolatedVertexRemovalTest, 3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    add_extra_vertex_and_check(mesh);
}

