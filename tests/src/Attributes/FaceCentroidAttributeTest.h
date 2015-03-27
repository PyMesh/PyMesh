/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <TestBase.h>

class FaceCentroidAttributeTest : public TestBase {
    protected:
        virtual MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_centroid");
            return r;
        }

        void assert_colinear(VectorF p1, VectorF p2, VectorF p3) {
            Float l1 = (p2 - p1).norm();
            Float l2 = (p3 - p1).norm();
            Float dot = (p2 - p1).dot(p3 - p1);
            ASSERT_FLOAT_EQ(dot, l1 * l2);
        }
};

TEST_F(FaceCentroidAttributeTest, 2D) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_TRUE(square->has_attribute("face_centroid"));
    VectorF centroids = square->get_attribute("face_centroid");

    const size_t dim = square->get_dim();
    const size_t num_faces = square->get_num_faces();
    const size_t vertex_per_face = square->get_vertex_per_face();
    ASSERT_EQ(3, vertex_per_face);
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = square->get_face(i);
        VectorF centroid = centroids.segment(i*dim, dim);
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t curr = j;
            size_t next = (j+1) % vertex_per_face;
            size_t prev = (j-1+vertex_per_face) % vertex_per_face;
            VectorF mid_edge_point = 0.5*(
                    square->get_vertex(face[next]) +
                    square->get_vertex(face[prev]));
            assert_colinear(square->get_vertex(face[curr]), centroid, mid_edge_point);
        }
    }
}

TEST_F(FaceCentroidAttributeTest, 3D) {
    MeshPtr cube = load_mesh("cube.msh");
    ASSERT_TRUE(cube->has_attribute("face_centroid"));
    VectorF centroids = cube->get_attribute("face_centroid");

    const size_t dim = cube->get_dim();
    const size_t num_faces = cube->get_num_faces();
    const size_t vertex_per_face = cube->get_vertex_per_face();
    ASSERT_EQ(3, vertex_per_face);
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = cube->get_face(i);
        VectorF centroid = centroids.segment(i*dim, dim);
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t curr = j;
            size_t next = (j+1) % vertex_per_face;
            size_t prev = (j-1+vertex_per_face) % vertex_per_face;
            VectorF mid_edge_point = 0.5 * (
                    cube->get_vertex(face[next]) +
                    cube->get_vertex(face[prev]));

            assert_colinear(cube->get_vertex(face[curr]), centroid, mid_edge_point);
        }
    }
}
