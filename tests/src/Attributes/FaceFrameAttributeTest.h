/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class FaceFrameAttributeTest : public TestBase {
    protected:
        void check_frame_field(MeshPtr mesh) {
            ASSERT_TRUE(mesh->has_attribute("face_frame"));
            ASSERT_TRUE(mesh->has_attribute("face_normal"));
            const auto frame_field = mesh->get_attribute("face_frame");
            const auto normals = mesh->get_attribute("face_normal");
            const size_t num_faces = mesh->get_num_faces();
            for (size_t i=0; i<num_faces; i++) {
                const Vector3F n = normals.segment<3>(i*3);
                const Vector3F e0 = frame_field.segment<3>(i*6);
                const Vector3F e1 = frame_field.segment<3>(i*6+3);
                ASSERT_NEAR(0.0, n.dot(e0), 1e-6);
                ASSERT_NEAR(0.0, n.dot(e1), 1e-6);
                ASSERT_NEAR(0.0, e0.dot(e1), 1e-6);
            }
        }
};

TEST_F(FaceFrameAttributeTest, cube) {
    MeshPtr cube = load_mesh("cube.obj");
    cube->add_attribute("face_frame");
    cube->add_attribute("face_normal");
    check_frame_field(cube);
}

TEST_F(FaceFrameAttributeTest, ball) {
    MeshPtr mesh = load_mesh("ball.msh");
    mesh->add_attribute("face_frame");
    mesh->add_attribute("face_normal");
    check_frame_field(mesh);
}

TEST_F(FaceFrameAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.mesh");
    mesh->add_attribute("face_frame");
    mesh->add_attribute("face_normal");
    check_frame_field(mesh);
}

TEST_F(FaceFrameAttributeTest, suzanne) {
    MeshPtr mesh = load_mesh("suzanne.obj");
    mesh->add_attribute("face_frame");
    mesh->add_attribute("face_normal");
    check_frame_field(mesh);
}
