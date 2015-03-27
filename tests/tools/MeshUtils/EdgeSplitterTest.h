/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <functional>

#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>
#include <TestBase.h>

#include <MeshUtils/EdgeSplitter.h>

class EdgeSplitterTest : public TestBase {
    protected:
        void ASSERT_MESH_IS_VALID(const MatrixFr& vertices, const MatrixIr& faces) {
            const size_t num_vertices = vertices.rows();
            ASSERT_TRUE((faces.array() < num_vertices).all());
        }
};

TEST_F(EdgeSplitterTest, SingleTriangle) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
               10.0, 0.0, 0.0,
                5.0, 1.0, 0.0;
    MatrixIr faces(1, 3);
    faces << 0, 1, 2;

    EdgeSplitter splitter(vertices, faces);
    EdgeSplitter::IndicatorFunc f =
        [=](const VectorF& v0, const VectorF& v1) {
            return (v0[1] == 0.0) &&
                (v0[2] == 0.0) &&
                (v1[1] == 0.0) &&
                (v1[2] == 0.0);
        };
    splitter.run(f, 1.0);

    auto result_vertices = splitter.get_vertices();
    auto result_faces = splitter.get_faces();

    ASSERT_EQ(18, result_vertices.rows());
    ASSERT_EQ(16, result_faces.rows());
    ASSERT_MESH_IS_VALID(result_vertices, result_faces);
}

TEST_F(EdgeSplitterTest, DoubleTriangles) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
               10.0, 0.0, 0.0,
                5.0, 1.0, 0.0,
                5.0,-1.0, 0.0;

    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             1, 0, 3;

    EdgeSplitter splitter(vertices, faces);
    EdgeSplitter::IndicatorFunc f =
        [=](const VectorF& v0, const VectorF& v1) {
            return (v0[1] == 0.0) &&
                (v0[2] == 0.0) &&
                (v1[1] == 0.0) &&
                (v1[2] == 0.0);
        };
    splitter.run(f, 1.0);

    auto result_vertices = splitter.get_vertices();
    auto result_faces = splitter.get_faces();

    ASSERT_EQ(19, result_vertices.rows());
    ASSERT_EQ(32, result_faces.rows());
    ASSERT_MESH_IS_VALID(result_vertices, result_faces);
}

TEST_F(EdgeSplitterTest, cube) {
    MeshPtr mesh = load_mesh("cube.obj");
    auto vertices = MatrixUtils::reshape<MatrixFr>(
            mesh->get_vertices(),
            mesh->get_num_vertices(),
            mesh->get_dim());
    auto faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(),
            mesh->get_num_faces(),
            mesh->get_vertex_per_face());

    auto bbox_min = vertices.colwise().minCoeff();
    EdgeSplitter splitter(vertices, faces);
    EdgeSplitter::IndicatorFunc f =
        [=](const VectorF& v0, const VectorF& v1) {
            return (v0[0] == bbox_min[0] && v1[0] == bbox_min[0]);
        };

    splitter.run(f, 1.0);
    splitter.run(f, 1.0);
    auto result_vertices = splitter.get_vertices();
    auto result_faces = splitter.get_faces();

    write_mesh_raw("box_split.obj", result_vertices, result_faces);
    ASSERT_MESH_IS_VALID(result_vertices, result_faces);
}
