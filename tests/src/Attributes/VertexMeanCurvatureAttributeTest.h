/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <TestBase.h>

class VertexMeanCurvatureAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("vertex_mean_curvature");
            return r;
        }
};

TEST_F(VertexMeanCurvatureAttributeTest, ConvexShape) {
    MeshPtr cube = load_mesh("cube.msh");
    const size_t num_vertices = cube->get_num_vertices();
    VectorF mean_curvature = cube->get_attribute("vertex_mean_curvature");
    ASSERT_EQ(num_vertices, mean_curvature.size());
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_LE(0.0, mean_curvature[i]);
    }
}
