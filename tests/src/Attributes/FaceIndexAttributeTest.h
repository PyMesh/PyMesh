/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <TestBase.h>

class FaceIndexAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_index");
            return r;
        }
};

TEST_F(FaceIndexAttributeTest, cube) {
    MeshPtr cube = load_mesh("cube.msh");
    const size_t num_faces = cube->get_num_faces();
    const VectorF& indices = cube->get_attribute("face_index");
    for (size_t i=0; i<num_faces; i++) {
        ASSERT_FLOAT_EQ(Float(i), indices[i]);
    }
}
