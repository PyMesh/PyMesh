/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VertexValanceAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("vertex_valance");
            return r;
        }
};

TEST_F(VertexValanceAttributeTest, Quad) {
    MeshPtr quad = load_mesh("quad.obj");
    ASSERT_TRUE(quad->has_attribute("vertex_valance"));
    VectorF valance = quad->get_attribute("vertex_valance");
    ASSERT_EQ(4, valance.size());
    ASSERT_FLOAT_EQ(2.0, valance.minCoeff());
    ASSERT_FLOAT_EQ(2.0, valance.maxCoeff());
}

TEST_F(VertexValanceAttributeTest, Hex) {
    MeshPtr quad = load_mesh("hex.msh");
    ASSERT_TRUE(quad->has_attribute("vertex_valance"));
    VectorF valance = quad->get_attribute("vertex_valance");
    ASSERT_EQ(8, valance.size());
    ASSERT_FLOAT_EQ(3.0, valance.minCoeff());
    ASSERT_FLOAT_EQ(3.0, valance.maxCoeff());
}

TEST_F(VertexValanceAttributeTest, Tet) {
    MeshPtr quad = load_mesh("tet.msh");
    ASSERT_TRUE(quad->has_attribute("vertex_valance"));
    VectorF valance = quad->get_attribute("vertex_valance");
    ASSERT_EQ(4, valance.size());
    ASSERT_FLOAT_EQ(3.0, valance.minCoeff());
    ASSERT_FLOAT_EQ(3.0, valance.maxCoeff());
}
