/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <string>
#include <cmath>

#include <TestBase.h>

class FaceAspectRatioAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_aspect_ratio");
            return r;
        }
};

TEST_F(FaceAspectRatioAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.obj");
    VectorF aspect_ratio = mesh->get_attribute("face_aspect_ratio");
    ASSERT_FLOAT_EQ(1.0, aspect_ratio.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, aspect_ratio.minCoeff());
}

TEST_F(FaceAspectRatioAttributeTest, quad) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    VectorF aspect_ratio = mesh->get_attribute("face_aspect_ratio");

    // Ground truth aspect ratio for right triangles.
    const Float ar = (sqrt(2.0) + 1) / 2.0;

    ASSERT_FLOAT_EQ(ar, aspect_ratio.maxCoeff());
    ASSERT_FLOAT_EQ(ar, aspect_ratio.minCoeff());
}

TEST_F(FaceAspectRatioAttributeTest, degenerate) {
    MatrixFr vertices(2, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0;

    MatrixIr faces(1, 3);
    faces << 0, 1, 1;

    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    mesh->add_attribute("face_aspect_ratio");
    VectorF aspect_ratio = mesh->get_attribute("face_aspect_ratio");

    ASSERT_FALSE(std::isfinite(aspect_ratio[0]));
}

TEST_F(FaceAspectRatioAttributeTest, degenerate2) {
    MatrixFr vertices(2, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0;

    MatrixIr faces(1, 3);
    faces << 1, 1, 1;

    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    mesh->add_attribute("face_aspect_ratio");
    VectorF aspect_ratio = mesh->get_attribute("face_aspect_ratio");

    ASSERT_FALSE(std::isfinite(aspect_ratio[0]));
}
