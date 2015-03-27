/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <TestBase.h>

class VertexIndexAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("vertex_index");
            return r;
        }
};

TEST_F(VertexIndexAttributeTest, cube) {
    MeshPtr cube = load_mesh("cube.msh");
    const size_t num_vertices = cube->get_num_vertices();
    const VectorF& indices = cube->get_attribute("vertex_index");
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_FLOAT_EQ(Float(i), indices[i]);
    }
}
