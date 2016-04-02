/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <string>
#include <cmath>

#include <TestBase.h>

class FaceIncircleRadiusAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_incircle_radius");
            return r;
        }

        void check_incircle_radii(MeshPtr mesh) {
            if (!mesh->has_attribute("face_incircle_radius")) {
                mesh->add_attribute("face_incircle_radius");
            }
            if (!mesh->has_attribute("face_incircle_center")) {
                mesh->add_attribute("face_incircle_center");
            }

            const VectorF& centers = mesh->get_attribute("face_incircle_center");
            const VectorF& radii = mesh->get_attribute("face_incircle_radius");
            const size_t dim = mesh->get_dim();
            const size_t num_faces = mesh->get_num_faces();
            for (size_t i=0; i<num_faces; i++) {
                VectorI face = mesh->get_face(i);
                VectorF center = centers.segment(i*dim, dim);
                check_incircle_radius(radii[i], center,
                        mesh->get_vertex(face[0]),
                        mesh->get_vertex(face[1]),
                        mesh->get_vertex(face[2]));
            }
        }

        void check_incircle_radius(
                const Float r,
                const VectorF& center,
                const VectorF& p1,
                const VectorF& p2,
                const VectorF& p3) {
            const Float cp1 = (center - p1).norm();
            const Float cp2 = (center - p2).norm();
            const Float cp3 = (center - p3).norm();
            const Float o1 = (p2 - p3).norm();
            const Float o2 = (p1 - p3).norm();
            const Float o3 = (p1 - p2).norm();

            ASSERT_FLOAT_EQ(o1 + o2 + o3,
                    2 * sqrt(cp1*cp1-r*r) +
                    2 * sqrt(cp2*cp2-r*r) +
                    2 * sqrt(cp3*cp3-r*r));
        }
};

TEST_F(FaceIncircleRadiusAttributeTest, 2D) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_TRUE(square->has_attribute("face_incircle_radius"));
    check_incircle_radii(square);
}

TEST_F(FaceIncircleRadiusAttributeTest, 3D) {
    MeshPtr tet = load_mesh("tet.obj");
    ASSERT_TRUE(tet->has_attribute("face_incircle_radius"));
    check_incircle_radii(tet);
}

TEST_F(FaceIncircleRadiusAttributeTest, DegenerateTriangle) {
    MatrixFr vertices(2, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0;
    MatrixIr faces(1, 3);
    faces << 0, 1, 1;
    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    check_incircle_radii(mesh);
}

TEST_F(FaceIncircleRadiusAttributeTest, DegenerateTriangle2) {
    MatrixFr vertices(2, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0;
    MatrixIr faces(1, 3);
    faces << 1, 1, 1;
    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    check_incircle_radii(mesh);
}
