/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <iostream>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <TestBase.h>

class IsolatedVertexRemovalTest : public TestBase {
    protected:
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
            VectorI  ori_vertex_indices = remover.get_ori_vertex_indices();
            VectorI  tar_vertex_indices =
                MatrixUtils::range<VectorI>(num_vertices).array() + 1;

            ASSERT_MATRIX_EQ(vertices, result_vertices);
            ASSERT_MATRIX_EQ(faces, result_faces);
            ASSERT_MATRIX_EQ(tar_vertex_indices, ori_vertex_indices);
        }
};

TEST_F(IsolatedVertexRemovalTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    add_extra_vertex_and_check(mesh);
}

TEST_F(IsolatedVertexRemovalTest, 3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    add_extra_vertex_and_check(mesh);
}

