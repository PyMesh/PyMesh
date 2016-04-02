/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <string>
#include <cmath>

#include <TestBase.h>

class FaceCircumRadiusAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            return r;
        }

        void check_circumradii(MeshPtr mesh) {
            if (!mesh->has_attribute("face_circumcenter")) {
                mesh->add_attribute("face_circumcenter");
            }
            if (!mesh->has_attribute("face_circumradius")) {
                mesh->add_attribute("face_circumradius");
            }

            VectorF circumcenters = mesh->get_attribute("face_circumcenter");
            VectorF circumradii = mesh->get_attribute("face_circumradius");

            const size_t dim = mesh->get_dim();
            const size_t num_faces = mesh->get_num_faces();
            for (size_t i=0; i<num_faces; i++) {
                VectorI face = mesh->get_face(i);
                VectorF center = circumcenters.segment(i*dim, dim);
                assert_equal_dist_to_vertices(
                        circumradii[i], center,
                        mesh->get_vertex(face[0]),
                        mesh->get_vertex(face[1]),
                        mesh->get_vertex(face[2]));
            }
        }

        void assert_equal_dist_to_vertices(
                Float r,
                const VectorF& center,
                const VectorF& p1,
                const VectorF& p2,
                const VectorF& p3) {
            if (!std::isfinite(r) || !center.allFinite()) {
                ASSERT_TRUE(!std::isfinite(r));
                ASSERT_TRUE(!center.allFinite());
                return;
            }

            Float d1 = (center - p1).norm();
            Float d2 = (center - p2).norm();
            Float d3 = (center - p3).norm();

            ASSERT_FLOAT_EQ(r, d1);
            ASSERT_FLOAT_EQ(r, d2);
            ASSERT_FLOAT_EQ(r, d3);
        }
};

TEST_F(FaceCircumRadiusAttributeTest, 2D) {
    MeshPtr square = load_mesh("square_2D.obj");
    check_circumradii(square);
}

TEST_F(FaceCircumRadiusAttributeTest, 3D) {
    MeshPtr tet = load_mesh("tet.obj");
    check_circumradii(tet);
}

TEST_F(FaceCircumRadiusAttributeTest, degenerate) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                1.0, 1.0, 0.0;

    MatrixIr faces(1, 3);
    faces << 0, 1, 1;

    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    check_circumradii(mesh);
}

TEST_F(FaceCircumRadiusAttributeTest, degenerate2) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                1.0, 1.0, 0.0;

    MatrixIr faces(1, 3);
    faces << 1, 1, 1;

    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    check_circumradii(mesh);
}
