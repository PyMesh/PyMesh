/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <cmath>

#include <TestBase.h>

class FaceVoronoiAreaAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_area");
            r->add_attribute("face_voronoi_area");
            return r;
        }
};

TEST_F(FaceVoronoiAreaAttributeTest, 2D) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_EQ(3, square->get_vertex_per_face());
    ASSERT_TRUE(square->has_attribute("face_voronoi_area"));
    ASSERT_TRUE(square->has_attribute("face_area"));

    const VectorF & areas = square->get_attribute("face_area");
    const VectorF& voronoi_areas = square->get_attribute("face_voronoi_area");

    const size_t num_faces = square->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        Float area = areas[i];
        VectorF voronoi_area = voronoi_areas.segment(i*3,3);
        ASSERT_TRUE(std::isfinite(voronoi_area[0]));
        ASSERT_TRUE(std::isfinite(voronoi_area[1]));
        ASSERT_TRUE(std::isfinite(voronoi_area[2]));
        ASSERT_FLOAT_EQ(area, voronoi_area.sum());
    }
}

TEST_F(FaceVoronoiAreaAttributeTest, 3D) {
    MeshPtr cube = load_mesh("cube.msh");
    ASSERT_EQ(3, cube->get_vertex_per_face());
    ASSERT_TRUE(cube->has_attribute("face_voronoi_area"));
    ASSERT_TRUE(cube->has_attribute("face_area"));

    const VectorF & areas = cube->get_attribute("face_area");
    const VectorF& voronoi_areas = cube->get_attribute("face_voronoi_area");

    const size_t num_faces = cube->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        Float area = areas[i];
        VectorF voronoi_area = voronoi_areas.segment(i*3,3);
        ASSERT_TRUE(std::isfinite(voronoi_area[0]));
        ASSERT_TRUE(std::isfinite(voronoi_area[1]));
        ASSERT_TRUE(std::isfinite(voronoi_area[2]));
        ASSERT_FLOAT_EQ(area, voronoi_area.sum());
    }
}

TEST_F(FaceVoronoiAreaAttributeTest, 3Dtet) {
    MeshPtr tet = load_mesh("tet.obj");
    ASSERT_EQ(3, tet->get_vertex_per_face());
    ASSERT_TRUE(tet->has_attribute("face_voronoi_area"));
    ASSERT_TRUE(tet->has_attribute("face_area"));

    const VectorF & areas = tet->get_attribute("face_area");
    const VectorF& voronoi_areas = tet->get_attribute("face_voronoi_area");

    const size_t num_faces = tet->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        Float area = areas[i];
        VectorF voronoi_area = voronoi_areas.segment(i*3,3);

        ASSERT_TRUE(std::isfinite(voronoi_area[0]));
        ASSERT_TRUE(std::isfinite(voronoi_area[1]));
        ASSERT_TRUE(std::isfinite(voronoi_area[2]));

        // By symmetry, voronoi area is 1/3 of the total area.
        ASSERT_FLOAT_EQ(1.0/3.0, voronoi_area[0] / area);
        ASSERT_FLOAT_EQ(1.0/3.0, voronoi_area[1] / area);
        ASSERT_FLOAT_EQ(1.0/3.0, voronoi_area[2] / area);

        ASSERT_FLOAT_EQ(area, voronoi_area.sum());
    }
}

TEST_F(FaceVoronoiAreaAttributeTest, 2DPerVertex) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_EQ(3, square->get_vertex_per_face());
    ASSERT_TRUE(square->has_attribute("face_voronoi_area"));
    ASSERT_TRUE(square->has_attribute("face_area"));

    const VectorF& voronoi_areas = square->get_attribute("face_voronoi_area");

    const size_t num_vertices = square->get_num_vertices();
    const size_t num_faces = square->get_num_faces();

    VectorF vertex_voronoi = VectorF::Zero(num_vertices);
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = square->get_face(i);
        VectorF voronoi_area = voronoi_areas.segment(i*3,3);

        vertex_voronoi[face[0]] += voronoi_area[0];
        vertex_voronoi[face[1]] += voronoi_area[1];
        vertex_voronoi[face[2]] += voronoi_area[2];
    }

    Float total_area = vertex_voronoi.sum();
    for (size_t i=0; i<num_vertices; i++) {
        ASSERT_FLOAT_EQ(total_area/num_vertices, vertex_voronoi[i]);
    }
}
