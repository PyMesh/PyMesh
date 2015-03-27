/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <functional>

#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>
#include <TestBase.h>

#include <Wires/Misc/BoundaryRemesher.h>

class BoundaryRemesherTest : public TestBase {
    protected:
        void ASSERT_BBOX_EQ(const MatrixFr& v1, const MatrixFr& v2) {
            VectorF bbox_min_1 = v1.colwise().minCoeff();
            VectorF bbox_max_1 = v1.colwise().maxCoeff();
            VectorF bbox_min_2 = v2.colwise().minCoeff();
            VectorF bbox_max_2 = v2.colwise().maxCoeff();

            ASSERT_MATRIX_EQ(bbox_min_1, bbox_min_2);
            ASSERT_MATRIX_EQ(bbox_max_1, bbox_max_2);
        }
};

TEST_F(BoundaryRemesherTest, Creation) {
    MeshPtr mesh = load_mesh("cube.obj");
    auto vertices = MatrixUtils::reshape<MatrixFr>(
            mesh->get_vertices(),
            mesh->get_num_vertices(),
            mesh->get_dim());
    auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    BoundaryRemesher remesher(vertices, faces);

    std::function<bool(const VectorF& v) > f(
            [=](const VectorF& v) { return v[0] >= 1.0; });
    remesher.remesh(f, 0.1);

    const auto& remeshed_vertices = remesher.get_vertices();
    const auto& remeshed_faces = remesher.get_faces();
    write_mesh_raw("remeshed_box.obj", remeshed_vertices, remeshed_faces);
    ASSERT_BBOX_EQ(vertices, remeshed_vertices);
    ASSERT_LT(0, remeshed_faces.rows());
}
