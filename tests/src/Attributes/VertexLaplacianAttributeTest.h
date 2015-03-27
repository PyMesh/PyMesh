/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <TestBase.h>

class VertexLaplacianAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("vertex_laplacian");
            return r;
        }
};

TEST_F(VertexLaplacianAttributeTest, ConvexShape) {
    MeshPtr cube = load_mesh("cube.msh");
    cube->add_attribute("vertex_normal");
    VectorF laplacian = cube->get_attribute("vertex_laplacian");
    VectorF normal = cube->get_attribute("vertex_normal");
    ASSERT_LT(0.0, laplacian.dot(normal));
}

TEST_F(VertexLaplacianAttributeTest, LaplacianSumCube) {
    MeshPtr cube = load_mesh("cube.msh");
    VectorF laplacian = cube->get_attribute("vertex_laplacian");
    ASSERT_FLOAT_EQ(0.0, laplacian.sum());
}

TEST_F(VertexLaplacianAttributeTest, LaplacianSumTet) {
    MeshPtr tet = load_mesh("tet.obj");
    VectorF laplacian = tet->get_attribute("vertex_laplacian");
    ASSERT_FLOAT_EQ(0.0, laplacian.sum());
}
