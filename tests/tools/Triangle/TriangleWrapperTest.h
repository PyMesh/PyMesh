/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE

#include <Triangle/TriangleWrapper.h>
#include <IO/MeshWriter.h>
#include <Misc/MatrixIO.h>

#include <TestBase.h>

class TriangleWrapperTest : public TestBase {
    protected:
        virtual void SetUp() {
            m_vertices.resize(8, 2);
            m_vertices << 0.0, 0.0,
                          1.0, 0.0,
                          1.0, 1.0,
                          0.0, 1.0,
                          0.2, 0.2,
                          0.8, 0.2,
                          0.8, 0.8,
                          0.2, 0.8;

            m_segments.resize(8, 2);
            m_segments << 0, 1,
                          1, 2,
                          2, 3,
                          3, 0,
                          5, 4,
                          6, 5,
                          7, 6,
                          4, 7;
            m_holes.resize(1, 2);
            m_holes << 0.45, 0.5;
            TestBase::SetUp();
        }

        void save_mesh(const std::string& file_name,
                MatrixFr& vertices, MatrixIr& faces) {
            write_mesh_raw(file_name, vertices, faces);
        }

        bool inside_triangle(const VectorF& p,
                const VectorF& v0, const VectorF& v1, const VectorF& v2) {
            VectorF e0 = p - v0;
            VectorF e1 = p - v1;
            VectorF e2 = p - v2;

            Float area_01 = e0.x() * e1.y() - e0.y() * e1.x();
            Float area_12 = e1.x() * e2.y() - e1.y() * e2.x();
            Float area_20 = e2.x() * e0.y() - e2.y() * e0.x();
            return ((area_01 >= 0.0) && (area_12 >= 0.0) && (area_20 >= 0.0));
        }

        void ASSERT_OUTSIDE(const VectorF& p, MatrixFr& vertices, MatrixIr& faces) {
            const size_t num_faces = faces.rows();
            for (size_t i=0; i<num_faces; i++) {
                const VectorI& face = faces.row(i);
                const VectorF& v0 = vertices.row(face[0]);
                const VectorF& v1 = vertices.row(face[1]);
                const VectorF& v2 = vertices.row(face[2]);

                ASSERT_FALSE(inside_triangle(p, v0, v1, v2));
            }
        }

        void ASSERT_INSIDE(const VectorF& p, MatrixFr& vertices, MatrixIr& faces) {
            bool is_inside = false;
            const size_t num_faces = faces.rows();
            for (size_t i=0; i<num_faces; i++) {
                const VectorI& face = faces.row(i);
                const VectorF& v0 = vertices.row(face[0]);
                const VectorF& v1 = vertices.row(face[1]);
                const VectorF& v2 = vertices.row(face[2]);

                is_inside = is_inside || inside_triangle(p, v0, v1, v2);
            }
            ASSERT_TRUE(is_inside);
        }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_segments;
        MatrixFr m_holes;
};

TEST_F(TriangleWrapperTest, Creation) {
    TriangleWrapper tri;
    tri.set_points(m_vertices);
    tri.set_segments(m_segments);
    tri.set_max_area(0.1);
    tri.set_verbosity(0);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), m_vertices.rows());
    ASSERT_GT(faces.rows(), 0);

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_FLOAT_EQ(0.0, bbox_min.minCoeff());
    ASSERT_FLOAT_EQ(0.0, bbox_min.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.minCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.maxCoeff());
    ASSERT_INSIDE(m_holes.row(0), vertices, faces);
}

TEST_F(TriangleWrapperTest, Holes) {
    TriangleWrapper tri;
    tri.set_points(m_vertices);
    tri.set_segments(m_segments);
    tri.set_holes(m_holes);
    tri.set_max_area(0.1);
    tri.set_verbosity(0);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), m_vertices.rows());
    ASSERT_GT(faces.rows(), 0);

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_FLOAT_EQ(0.0, bbox_min.minCoeff());
    ASSERT_FLOAT_EQ(0.0, bbox_min.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.minCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.maxCoeff());
    ASSERT_OUTSIDE(m_holes.row(0), vertices, faces);
}

TEST_F(TriangleWrapperTest, AutoHoleDetection) {
    TriangleWrapper tri;
    tri.set_points(m_vertices);
    tri.set_segments(m_segments);
    tri.set_max_area(0.1);
    tri.set_verbosity(0);
    tri.set_auto_hole_detection(true);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), m_vertices.rows());
    ASSERT_GT(faces.rows(), 0);

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_FLOAT_EQ(0.0, bbox_min.minCoeff());
    ASSERT_FLOAT_EQ(0.0, bbox_min.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.minCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.maxCoeff());
    ASSERT_OUTSIDE(m_holes.row(0), vertices, faces);
}

TEST_F(TriangleWrapperTest, 3D) {
    size_t num_pts = m_vertices.rows();
    MatrixFr vertices_3d(num_pts, 3);
    vertices_3d.block(0,0,num_pts,2) = m_vertices;
    vertices_3d.block(0,2,num_pts,1) = VectorF::Ones(num_pts);

    TriangleWrapper tri;
    tri.set_points(vertices_3d);
    tri.set_segments(m_segments);
    tri.set_max_area(0.1);
    tri.set_verbosity(0);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), num_pts);
    ASSERT_EQ(3, vertices.cols());
    ASSERT_GT(faces.rows(), 0);

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_FLOAT_EQ(0.0, bbox_min[0]);
    ASSERT_FLOAT_EQ(0.0, bbox_min[1]);
    ASSERT_FLOAT_EQ(1.0, bbox_max[0]);
    ASSERT_FLOAT_EQ(1.0, bbox_max[1]);
}

TEST_F(TriangleWrapperTest, Refine) {
    TriangleWrapper tri;
    tri.set_points(m_vertices);
    tri.set_segments(m_segments);
    tri.set_holes(m_holes);
    tri.set_max_area(0.1);
    tri.set_verbosity(0);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();
    const size_t num_faces_before = faces.rows();

    TriangleWrapper refiner;
    refiner.set_points(vertices);
    refiner.set_triangles(faces);
    refiner.set_max_area(0.01);
    refiner.set_verbosity(0);
    refiner.run();

    vertices = refiner.get_vertices();
    faces = refiner.get_faces();
    const size_t num_faces_after = faces.rows();

    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();

    ASSERT_GT(num_faces_after, num_faces_before);
    ASSERT_FLOAT_EQ(0.0, bbox_min.minCoeff());
    ASSERT_FLOAT_EQ(0.0, bbox_min.maxCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.minCoeff());
    ASSERT_FLOAT_EQ(1.0, bbox_max.maxCoeff());
    ASSERT_OUTSIDE(m_holes.row(0), vertices, faces);
}

TEST_F(TriangleWrapperTest, 3DWithHoles) {
    size_t num_pts = m_vertices.rows();
    MatrixFr vertices_3d(num_pts, 3);
    vertices_3d.block(0,0,num_pts,2) = m_vertices;
    vertices_3d.block(0,2,num_pts,1) = VectorF::Ones(num_pts);

    MatrixFr holes(1, 3);
    holes << m_holes(0,0), m_holes(0,1), 1.0;

    TriangleWrapper tri;
    tri.set_points(vertices_3d);
    tri.set_segments(m_segments);
    tri.set_max_area(0.01);
    tri.set_verbosity(0);
    tri.set_auto_hole_detection(true);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_GE(vertices.rows(), m_vertices.rows());
    ASSERT_GT(faces.rows(), 0);

    ASSERT_OUTSIDE(holes.row(0), vertices, faces);
}

TEST_F(TriangleWrapperTest, DISABLED_debug) {
    MatrixFr input_vertices = MatrixIO::load_matrix<MatrixFr>(
            TestBase::m_data_dir + "/v1.npy");
    MatrixIr input_loops = MatrixIO::load_matrix<MatrixIr>(
            TestBase::m_data_dir + "/e1.npy");
    //std::cout << input_vertices.rows() << std::endl;
    //std::cout << input_loops.rows() << std::endl;
    //std::cout << input_vertices << std::endl;

    TriangleWrapper tri;
    tri.set_points(input_vertices);
    tri.set_segments(input_loops);
    tri.set_max_area(0.25);
    tri.set_verbosity(0);
    tri.set_split_boundary(false);
    tri.set_auto_hole_detection(true);
    tri.set_max_num_steiner_points(0);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();
    //std::cout << vertices.rows() << std::endl;
    //std::cout << faces.rows() << std::endl;
    //std::cout << vertices << std::endl;


    //save_mesh("triangle_debug_1.obj", vertices, faces);
    ASSERT_EQ(input_vertices.rows(), vertices.rows());
}

TEST_F(TriangleWrapperTest, debug2) {
    MatrixFr input_vertices = MatrixIO::load_matrix<MatrixFr>(
            TestBase::m_data_dir + "/v2.npy");
    MatrixIr input_loops = MatrixIO::load_matrix<MatrixIr>(
            TestBase::m_data_dir + "/e2.npy");
    //std::cout << input_vertices.rows() << std::endl;
    //std::cout << input_loops.rows() << std::endl;

    TriangleWrapper tri;
    tri.set_points(input_vertices);
    tri.set_segments(input_loops);
    tri.set_max_area(0.25);
    tri.set_verbosity(0);
    tri.set_split_boundary(false);
    tri.set_auto_hole_detection(true);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    //std::cout << vertices.rows() << std::endl;
    //std::cout << faces.rows() << std::endl;

    //save_mesh("triangle_debug_2.obj", vertices, faces);
}

TEST_F(TriangleWrapperTest, refinement) {
    MatrixFr input_vertices(4, 3);
    input_vertices <<
        -2.077770, 1.136427, 0.0,
        1.922230, 1.136427, 0.0,
        -2.077770, -1.122556, 0.0,
        1.922230, -1.122556, 0.0;
    MatrixIr input_faces(2, 3);
    input_faces <<
        2, 3, 0,
        1, 0, 3;

    TriangleWrapper tri;
    tri.set_points(input_vertices);
    tri.set_triangles(input_faces);
    tri.set_max_area(0.25);
    tri.set_verbosity(0);
    tri.run();

    MatrixFr vertices = tri.get_vertices();
    MatrixIr faces = tri.get_faces();

    ASSERT_LT(0, faces.rows());
}

TEST_F(TriangleWrapperTest, point_cloud) {
    MatrixFr points(4, 3);
    points << 0.0, 0.0, 0.0,
              1.0, 0.0, 0.0,
              1.0, 1.0, 0.0,
              0.0, 1.0, 0.0;
    TriangleWrapper tri;
    tri.set_points(points);
    tri.set_verbosity(0);
    tri.run();

    auto vertices = tri.get_vertices();
    auto faces = tri.get_faces();

    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(2, faces.rows());

    auto voro_vertices = tri.get_voronoi_vertices();
    auto voro_edges = tri.get_voronoi_edges();
    ASSERT_GE(voro_edges.minCoeff(), -1);
    ASSERT_LT(voro_edges.maxCoeff(), voro_vertices.rows());
}

TEST_F(TriangleWrapperTest, convex_hull) {
    MatrixFr points(4, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              0.0, 1.0,
              0.1, 0.1;
    MatrixIr segments(4, 2);
    segments << 0,1,
                0,2,
                1,3,
                2,3;
    TriangleWrapper tri;
    tri.set_points(points);
    tri.set_segments(segments);
    tri.set_verbosity(0);
    tri.set_split_boundary(false);
    tri.set_max_num_steiner_points(0);
    tri.keep_convex_hull(true);
    tri.run();

    auto vertices = tri.get_vertices();
    auto faces = tri.get_faces();

    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(3, faces.rows());
}

TEST_F(TriangleWrapperTest, conforming) {
    MatrixFr points(4, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              0.0, 1.0,
              0.001, 0.001;
    MatrixIr segments(4, 2);
    segments << 0,1,
                0,2,
                1,3,
                2,3;
    TriangleWrapper tri;
    tri.set_points(points);
    tri.set_segments(segments);
    tri.set_verbosity(0);
    tri.set_conforming_delaunay(true);
    tri.run();

    auto vertices = tri.get_vertices();
    auto faces = tri.get_faces();

    ASSERT_LT(4, vertices.rows());
    ASSERT_LT(2, faces.rows());
}

TEST_F(TriangleWrapperTest, self_intersecting_segments) {
    MatrixFr points(4, 2);
    points << 0.0, 0.0,
              1.0, 0.0,
              0.0, 1.0,
              1.0, 1.0;
    MatrixIr segments(2, 2);
    segments << 0,3,
                1,2;
    TriangleWrapper tri;
    tri.set_points(points);
    tri.set_segments(segments);
    tri.set_verbosity(0);
    tri.keep_convex_hull(true);
    tri.run();

    auto vertices = tri.get_vertices();
    auto faces = tri.get_faces();

    ASSERT_EQ(5, vertices.rows());
    ASSERT_EQ(4, faces.rows());
}

#endif
