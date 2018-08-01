/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <TestBase.h>

class EdgeSquaredLengthAttributeTest : public TestBase {
    protected:
};

TEST_F(EdgeSquaredLengthAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    const auto num_faces = mesh->get_num_faces();
    const auto vertex_per_face = mesh->get_vertex_per_face();
    mesh->add_attribute("edge_squared_length");
    ASSERT_TRUE(mesh->has_attribute("edge_squared_length"));
    VectorF sq_lengths = mesh->get_attribute("edge_squared_length");
    ASSERT_EQ(num_faces * vertex_per_face, sq_lengths.rows());
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            ASSERT_NEAR(sq_lengths[i*vertex_per_face+j], sq_lengths[0], 1e-5);
        }
    }
}

TEST_F(EdgeSquaredLengthAttributeTest, hex) {
    MeshPtr mesh = load_mesh("hex.msh");
    const auto num_faces = mesh->get_num_faces();
    const auto vertex_per_face = mesh->get_vertex_per_face();
    mesh->add_attribute("edge_squared_length");
    ASSERT_TRUE(mesh->has_attribute("edge_squared_length"));
    VectorF sq_lengths = mesh->get_attribute("edge_squared_length");
    ASSERT_EQ(num_faces * vertex_per_face, sq_lengths.rows());
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            ASSERT_NEAR(sq_lengths[i*vertex_per_face+j], sq_lengths[0], 1e-5);
        }
    }
}

TEST_F(EdgeSquaredLengthAttributeTest, empty) {
    MeshPtr mesh = load_mesh("empty.msh");
    mesh->add_attribute("edge_squared_length");
    ASSERT_TRUE(mesh->has_attribute("edge_squared_length"));
    VectorF sq_lengths = mesh->get_attribute("edge_squared_length");
    ASSERT_EQ(0, sq_lengths.rows());
}

