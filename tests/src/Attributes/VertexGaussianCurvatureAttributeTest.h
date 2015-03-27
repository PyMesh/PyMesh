/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <TestBase.h>

class VertexGaussianCurvatureAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("vertex_gaussian_curvature");
            return r;
        }
};

TEST_F(VertexGaussianCurvatureAttributeTest, CubeTest) {
    MeshPtr mesh = load_mesh("cube.obj");
    const size_t num_vertices = mesh->get_num_vertices();
    VectorF gaussian_curvature = mesh->get_attribute("vertex_gaussian_curvature");

    ASSERT_EQ(num_vertices, gaussian_curvature.size());
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_FLOAT_EQ(M_PI*0.5, gaussian_curvature[i]);
    }
}

TEST_F(VertexGaussianCurvatureAttributeTest, TetTest) {
    MeshPtr mesh = load_mesh("tet.obj");
    const size_t num_vertices = mesh->get_num_vertices();
    VectorF gaussian_curvature = mesh->get_attribute("vertex_gaussian_curvature");

    ASSERT_EQ(num_vertices, gaussian_curvature.size());
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_NEAR(M_PI, gaussian_curvature[i], 1e-5);
    }
}
