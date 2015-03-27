/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <string>

#include <TestBase.h>

class FaceNormalAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_normal");
            return r;
        }

        void ASSERT_ALMOST_ORTHOGONAL(const Vector3F& v1, const Vector3F& v2) {
            Float val = v1.dot(v2) / (v1.norm() * v2.norm());
            Float angle = acos(val);
            ASSERT_FLOAT_EQ(M_PI/2, angle);
        }

        void check_face_normal_attribute(MeshPtr mesh) {
            ASSERT_TRUE(mesh->has_attribute("face_normal"));
            VectorF normals = mesh->get_attribute("face_normal");

            const size_t dim = mesh->get_dim();
            const size_t num_faces = mesh->get_num_faces();
            const size_t vertex_per_face = mesh->get_vertex_per_face();

            for (size_t i=0; i<num_faces; i++) {
                VectorI face = mesh->get_face(i);
                Vector3F n = normals.segment(i*3, 3);

                for (size_t j=0; j<vertex_per_face; j++) {
                    Vector3F v1 = Vector3F::Zero();
                    Vector3F v2 = Vector3F::Zero();
                    v1.segment(0, dim) = mesh->get_vertex(face[j]);
                    v2.segment(0, dim) = mesh->get_vertex(face[(j+1)%vertex_per_face]);

                    ASSERT_ALMOST_ORTHOGONAL(n, v2 - v1);
                }
            }
        }
};

TEST_F(FaceNormalAttributeTest, 2D_triangle) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    check_face_normal_attribute(mesh);
}

TEST_F(FaceNormalAttributeTest, 3D_tet) {
    MeshPtr mesh = load_mesh("cube.msh");
    check_face_normal_attribute(mesh);
}

TEST_F(FaceNormalAttributeTest, 2D_quad) {
    MeshPtr mesh = load_mesh("quad.obj");
    check_face_normal_attribute(mesh);
}

TEST_F(FaceNormalAttributeTest, 3D_hex) {
    MeshPtr mesh = load_mesh("hex.msh");
    check_face_normal_attribute(mesh);
}
