/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <string>

#include <TestBase.h>

class FaceAreaAttributeTest : public TestBase {
    protected:
        virtual MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_area");
            r->add_attribute("face_centroid");
            return r;
        }

        Float get_triangle_area(
                const Vector3F& v0, const Vector3F& v1, const Vector3F& v2) {
            return ((v1 - v0).cross(v2 - v0)).norm() * 0.5;
        }

        void check_face_area(MeshPtr mesh) {
            ASSERT_TRUE(mesh->has_attribute("face_area"));
            ASSERT_TRUE(mesh->has_attribute("face_centroid"));
            VectorF areas = mesh->get_attribute("face_area");
            VectorF centroids = mesh->get_attribute("face_centroid");

            const size_t dim = mesh->get_dim();
            const size_t num_faces= mesh->get_num_faces();
            const size_t vertex_per_face = mesh->get_vertex_per_face();

            for (size_t i=0; i<num_faces; i++) {
                VectorI face = mesh->get_face(i);
                Vector3F centroid = Vector3F::Zero();
                centroid.segment(0, dim) = centroids.segment(i*dim, dim);
                Float area = 0.0;
                for (size_t j=0; j<vertex_per_face; j++) {
                    Vector3F v0 = Vector3F::Zero();
                    Vector3F v1 = Vector3F::Zero();
                    v0.segment(0, dim) = mesh->get_vertex(face[j]);
                    v1.segment(0, dim) = mesh->get_vertex(face[(j+1)%vertex_per_face]);
                    area += get_triangle_area(v0, v1, centroid);
                }

                ASSERT_FLOAT_EQ(area, areas[i]);
            }
        }
};

TEST_F(FaceAreaAttributeTest, 2D_triangle) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    check_face_area(mesh);
}

TEST_F(FaceAreaAttributeTest, 2D_quad) {
    MeshPtr mesh = load_mesh("quad.obj");
    check_face_area(mesh);
}

TEST_F(FaceAreaAttributeTest, 3D_tet) {
    MeshPtr mesh = load_mesh("cube.msh");
    check_face_area(mesh);
}

TEST_F(FaceAreaAttributeTest, 3D_hex) {
    MeshPtr mesh = load_mesh("hex.msh");
    check_face_area(mesh);
}
