/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#ifdef WITH_TETGEN

#include <TestBase.h>
#include <Tetrahedralization/TetrahedralizationEngine.h>

class TetGenEngineTest : public TestBase {
};

TEST_F(TetGenEngineTest, simple) {
    MatrixFr vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
             1.0, 0.0, 0.0,
             0.0, 1.0, 0.0,
             0.0, 0.0, 1.0;

    MatrixIr faces(4, 3);
    faces << 0, 2, 1,
             1, 3, 0,
             0, 3, 2,
             1, 2, 3;

    auto engine = TetrahedralizationEngine::create("tetgen");
    ASSERT_TRUE(bool(engine));

    engine->set_vertices(vertices);
    engine->set_faces(faces);
    engine->run();

    const auto out_vertices = engine->get_vertices();
    const auto out_faces = engine->get_faces();
    const auto out_voxels = engine->get_voxels();

    ASSERT_GE(out_vertices.rows(), 4);
    ASSERT_EQ(out_vertices.cols(), 3);
    ASSERT_GE(out_faces.rows(), 4);
    ASSERT_EQ(out_faces.cols(), 3);
    ASSERT_GT(out_voxels.rows(), 0);
}

TEST_F(TetGenEngineTest, cube) {
    auto mesh = load_mesh("cube.obj");
    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    auto engine = TetrahedralizationEngine::create("tetgen");
    ASSERT_TRUE(bool(engine));

    engine->set_vertices(vertices);
    engine->set_faces(faces);
    engine->set_cell_size(1.0);
    engine->run();

    const auto out_vertices = engine->get_vertices();
    const auto out_faces = engine->get_faces();
    const auto out_voxels = engine->get_voxels();

    ASSERT_GE(out_vertices.rows(), vertices.rows());
    ASSERT_EQ(out_vertices.cols(), 3);
    ASSERT_GE(out_faces.rows(), faces.rows());
    ASSERT_EQ(out_faces.cols(), 3);
    ASSERT_GT(out_voxels.rows(), 0);

    engine->set_cell_size(0.1);
    engine->run();
    const auto out_vertices_2 = engine->get_vertices();
    const auto out_faces_2 = engine->get_faces();
    const auto out_voxels_2 = engine->get_voxels();

    ASSERT_GT(out_vertices_2.rows(), out_vertices.rows());
    ASSERT_EQ(out_vertices_2.cols(), 3);
    ASSERT_GT(out_faces_2.rows(), out_faces.rows());
    ASSERT_EQ(out_faces_2.cols(), 3);
    ASSERT_GT(out_voxels_2.rows(), out_voxels.rows());
}

TEST_F(TetGenEngineTest, ball) {
    auto mesh = load_mesh("ball.msh");
    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    auto engine = TetrahedralizationEngine::create("tetgen");
    ASSERT_TRUE(bool(engine));

    engine->set_vertices(vertices);
    engine->set_faces(faces);
    engine->set_cell_size(0.173205);
    engine->run();

    const auto out_vertices = engine->get_vertices();
    const auto out_faces = engine->get_faces();
    const auto out_voxels = engine->get_voxels();

    ASSERT_GE(out_vertices.rows(), vertices.rows());
    ASSERT_EQ(out_vertices.cols(), 3);
    ASSERT_GE(out_faces.rows(), faces.rows());
    ASSERT_EQ(out_faces.cols(), 3);
    ASSERT_GT(out_voxels.rows(), 0);
}

#endif
