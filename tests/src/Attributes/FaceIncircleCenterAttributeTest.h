/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <cmath>

#include <TestBase.h>

class FaceIncircleCenterAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_incircle_center");
            return r;
        }

        void check_incircle_centers(MeshPtr mesh) {
            if (!mesh->has_attribute("face_incircle_center")) {
                mesh->add_attribute("face_incircle_center");
            }

            const VectorF& centers = mesh->get_attribute("face_incircle_center");
            const size_t dim = mesh->get_dim();
            const size_t num_faces = mesh->get_num_faces();
            for (size_t i=0; i<num_faces; i++) {
                VectorI face = mesh->get_face(i);
                VectorF center = centers.segment(i*dim, dim);
                check_incircle_center(center,
                        mesh->get_vertex(face[0]),
                        mesh->get_vertex(face[1]),
                        mesh->get_vertex(face[2]));
            }
        }

        void check_incircle_center(
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

            const Float ratio1 = cp1==0.0 ? 0.0 : cp1*cp1/(o2*o3);
            const Float ratio2 = cp2==0.0 ? 0.0 : cp2*cp2/(o1*o3);
            const Float ratio3 = cp3==0.0 ? 0.0 : cp3*cp3/(o1*o2);

            if (ratio1 != 0.0 || ratio2 != 0.0 || ratio2 != 0.0) {
                ASSERT_FLOAT_EQ(1.0, ratio1+ratio2+ratio3);
            }
        }
};

TEST_F(FaceIncircleCenterAttributeTest, 2D) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_TRUE(square->has_attribute("face_incircle_center"));
    check_incircle_centers(square);
}

TEST_F(FaceIncircleCenterAttributeTest, 3D) {
    MeshPtr cube = load_mesh("cube.obj");
    ASSERT_TRUE(cube->has_attribute("face_incircle_center"));
    check_incircle_centers(cube);
}

TEST_F(FaceIncircleCenterAttributeTest, DegenerateTriangle) {
    MatrixFr vertices(2, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0;
    MatrixIr faces(1, 3);
    faces << 0, 1, 1;
    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    check_incircle_centers(mesh);
}

TEST_F(FaceIncircleCenterAttributeTest, DegenerateTriangle2) {
    MatrixFr vertices(2, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0;
    MatrixIr faces(1, 3);
    faces << 1, 1, 1;
    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    check_incircle_centers(mesh);
}
