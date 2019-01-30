/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_TETGEN
#include <TetGen/TetgenWrapper.h>

#include <string>

#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>

#include <TestBase.h>

class TetgenWrapperTest : public TestBase {
    protected:
        MatrixFr extract_vertices(MeshPtr mesh) {
            const size_t num_vertices = mesh->get_num_vertices();
            const size_t dim = mesh->get_dim();
            return MatrixUtils::reshape<MatrixFr>(
                    mesh->get_vertices(), num_vertices, dim);
        }

        MatrixIr extract_faces(MeshPtr mesh) {
            const size_t num_faces = mesh->get_num_faces();
            const size_t vertex_per_face = mesh->get_vertex_per_face();
            return MatrixUtils::reshape<MatrixIr>(
                    mesh->get_faces(), num_faces, vertex_per_face);
        }
};

TEST_F(TetgenWrapperTest, Cube) {
    MeshPtr mesh = load_mesh("cube.obj");

    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    TetgenWrapper wrapper;
    wrapper.set_points(vertices);
    wrapper.set_triangles(faces);
    wrapper.set_split_boundary(false);
    wrapper.set_verbosity(0);
    wrapper.run();

    MatrixFr tet_vertices = wrapper.get_vertices();
    MatrixIr tet_faces = wrapper.get_faces();
    MatrixIr tet_voxels = wrapper.get_voxels();

    ASSERT_GE(tet_vertices.rows(), vertices.rows());
    ASSERT_GT(tet_faces.rows(), 0);
    ASSERT_GT(tet_voxels.rows(), 0);
}

#endif
