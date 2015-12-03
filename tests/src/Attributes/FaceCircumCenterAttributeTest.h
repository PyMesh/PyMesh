/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <cmath>

#include <TestBase.h>

class FaceCircumCenterAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_circumcenter");
            return r;
        }

        void assert_equal_dist_to_vertices(
                const VectorF& center,
                const VectorF& p1,
                const VectorF& p2,
                const VectorF& p3) {
            Float d1 = (center - p1).norm();
            Float d2 = (center - p2).norm();
            Float d3 = (center - p3).norm();

            ASSERT_TRUE(std::isfinite(d1));
            ASSERT_TRUE(std::isfinite(d2));
            ASSERT_TRUE(std::isfinite(d3));

            ASSERT_FLOAT_EQ(d1, d2);
            ASSERT_FLOAT_EQ(d1, d3);
        }
};

TEST_F(FaceCircumCenterAttributeTest, 2D) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_EQ(3, square->get_vertex_per_face());
    ASSERT_TRUE(square->has_attribute("face_circumcenter"));
    const VectorF& circumcenters = square->get_attribute("face_circumcenter");

    const size_t dim = square->get_dim();
    const size_t num_faces = square->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = square->get_face(i);
        VectorF center = circumcenters.segment(i*dim, dim);
        ASSERT_FLOAT_EQ(0.0, center[0]);
        ASSERT_FLOAT_EQ(0.0, center[1]);
        assert_equal_dist_to_vertices(center,
                square->get_vertex(face[0]),
                square->get_vertex(face[1]),
                square->get_vertex(face[2]));
    }
}

TEST_F(FaceCircumCenterAttributeTest, 3D) {
    MeshPtr cube = load_mesh("cube.msh");
    ASSERT_EQ(3, cube->get_vertex_per_face());
    ASSERT_TRUE(cube->has_attribute("face_circumcenter"));
    const VectorF& circumcenters = cube->get_attribute("face_circumcenter");

    const size_t dim = cube->get_dim();
    const size_t num_faces = cube->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = cube->get_face(i);
        VectorF center = circumcenters.segment(i*dim, dim);
        assert_equal_dist_to_vertices(center,
                cube->get_vertex(face[0]),
                cube->get_vertex(face[1]),
                cube->get_vertex(face[2]));
    }
}

TEST_F(FaceCircumCenterAttributeTest, equilateral_triangle) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.5, sqrt(3)*0.5, 0.0;

    MatrixIr faces(1, 3);
    faces << 0, 1, 2;

    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    mesh->add_attribute("face_circumcenter");
    ASSERT_TRUE(mesh->has_attribute("face_circumcenter"));
    const VectorF circumcenters = mesh->get_attribute("face_circumcenter");

    mesh->add_attribute("face_centroid");
    const VectorF centroid = mesh->get_attribute("face_centroid");

    ASSERT_EQ(3, circumcenters.size());
    ASSERT_FLOAT_EQ(centroid[0], circumcenters[0]);
    ASSERT_FLOAT_EQ(centroid[1], circumcenters[1]);
    ASSERT_FLOAT_EQ(centroid[2], circumcenters[2]);
}

TEST_F(FaceCircumCenterAttributeTest, right_triangle) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                1.0, 1.0, 0.0;

    MatrixIr faces(1, 3);
    faces << 0, 1, 2;

    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    mesh->add_attribute("face_circumcenter");
    ASSERT_TRUE(mesh->has_attribute("face_circumcenter"));
    const VectorF circumcenters = mesh->get_attribute("face_circumcenter");

    ASSERT_EQ(3, circumcenters.size());
    ASSERT_FLOAT_EQ(0.5, circumcenters[0]);
    ASSERT_FLOAT_EQ(0.5, circumcenters[1]);
    ASSERT_FLOAT_EQ(0.0, circumcenters[2]);
}

TEST_F(FaceCircumCenterAttributeTest, degenerated_triangle) {
    MatrixFr vertices(2, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0;

    MatrixIr faces(1, 3);
    faces << 0, 1, 1;

    MatrixIr voxels(0, 3);

    MeshPtr mesh = load_data(vertices, faces, voxels);
    mesh->add_attribute("face_circumcenter");
    ASSERT_TRUE(mesh->has_attribute("face_circumcenter"));
    const VectorF circumcenters = mesh->get_attribute("face_circumcenter");

    ASSERT_EQ(3, circumcenters.size());
    ASSERT_TRUE(circumcenters.hasNaN());
}

