/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <Triangulation/Triangulation.h>
#include <MeshUtils/MeshChecker.h>

class TriangulationTest : public TestBase {
    public:
        void assert_valid_triangulation(
                const MatrixFr& vertices, const MatrixIr& faces) {
            MatrixIr voxels(0, 4);
            MeshChecker checker(vertices, faces, voxels);
            ASSERT_TRUE(checker.is_oriented());
            ASSERT_TRUE(checker.is_edge_manifold());
            ASSERT_TRUE(checker.is_vertex_manifold());
            ASSERT_EQ(1, checker.get_num_connected_components());
        }

        void simple(const std::string& engine_name) {
            MatrixFr points(3, 2);
            points << 0.0, 0.0,
                   1.0, 0.0,
                   0.0, 1.0;

            Triangulation::Ptr t = Triangulation::create(engine_name);
            t->set_points(points);
            t->run();
            MatrixIr faces = t->get_faces();
            ASSERT_EQ(1, faces.rows());
            ASSERT_TRUE(
                    (faces(0, 0) == 0 && faces(0, 1) == 1 && faces(0, 2) == 2) ||
                    (faces(0, 0) == 1 && faces(0, 1) == 2 && faces(0, 2) == 0) ||
                    (faces(0, 0) == 2 && faces(0, 1) == 0 && faces(0, 2) == 1));
            assert_valid_triangulation(points, faces);
        }

        void simple2(const std::string& engine_name) {
            MatrixFr points(4, 2);
            points << 0.0, 0.0,
                   0.5, 0.0,
                   1.0, 0.0,
                   0.0, 1.0;

            Triangulation::Ptr t = Triangulation::create(engine_name);
            t->set_points(points);
            t->run();
            MatrixFr vertices = t->get_vertices();
            MatrixIr faces = t->get_faces();
            assert_valid_triangulation(vertices, faces);
        }

        void simple3(const std::string& engine_name) {
            MatrixFr points(4, 2);
            points << 0.0, 0.0,
                   1.0, 0.0,
                   1.0, 1.0,
                   0.0, 1.0;

            Triangulation::Ptr t = Triangulation::create(engine_name);
            t->set_points(points);
            t->run();
            MatrixFr vertices = t->get_vertices();
            MatrixIr faces = t->get_faces();
            ASSERT_EQ(2, faces.rows());
            assert_valid_triangulation(vertices, faces);
        }

        void degenerate(const std::string& engine_name) {
            MatrixFr points(3, 2);
            points << 0.0, 0.0,
                   0.5, 0.0,
                   1.0, 0.0;

            Triangulation::Ptr t = Triangulation::create(engine_name);
            t->set_points(points);
            t->run();
            MatrixIr faces = t->get_faces();
            ASSERT_EQ(0, faces.rows());
        }

        void intersecting_constraints(const std::string& engine_name) {
            MatrixFr points(4, 2);
            points << 1.0, 0.0,
                   -1.0, 0.0,
                   0.0, 1.0,
                   0.0,-1.0;
            Matrix2Ir segments(2, 2);
            segments << 0, 1,
                     2, 3;

            Triangulation::Ptr t = Triangulation::create(engine_name);
            t->set_points(points);
            t->set_segments(segments);
            t->run();

            MatrixFr vertices = t->get_vertices();
            MatrixIr faces = t->get_faces();
            assert_valid_triangulation(vertices, faces);
            ASSERT_LE(5, vertices.rows());
            ASSERT_LE(4, faces.rows());
        }

        void overlapping_segments(const std::string& engine_name) {
            MatrixFr points(6, 2);
            points << 1.0, 0.0,
                   -1.0, 0.0,
                   2.0, 0.0,
                   -2.0, 0.0,
                   0.0, 1.0,
                   0.0,-1.0;
            Matrix2Ir segments(3, 2);
            segments << 0, 1,
                     2, 3,
                     4, 5;

            Triangulation::Ptr t = Triangulation::create(engine_name);
            t->set_points(points);
            t->set_segments(segments);
            t->run();

            MatrixFr vertices = t->get_vertices();
            MatrixIr faces = t->get_faces();
            assert_valid_triangulation(vertices, faces);
            ASSERT_LE(7, vertices.rows());
            ASSERT_LE(8, faces.rows());
        }

        void near_degeneracy(const std::string& engine_name) {
            MatrixFr points(4, 2);
            points << -1.0, 0.0,
                   1.0, 0.0,
                   0.0, 1.0,
                   0.0, 0.0;
            Matrix2Ir segments(2, 2);
            segments << 0, 1,
                     2, 3;
            Triangulation::Ptr t = Triangulation::create(engine_name);

            Float tol = 1e-3;
            for (size_t i=0; i<20; i++) {
                points(3, 1) = tol;
                t->set_points(points);
                t->set_segments(segments);
                t->run();

                MatrixFr vertices = t->get_vertices();
                MatrixIr faces = t->get_faces();
                assert_valid_triangulation(vertices, faces);
                ASSERT_LE(4, vertices.rows());
                ASSERT_LE(3, faces.rows());

                tol /= 10.0;
            }
        }

        void duplicated_vertices(const std::string& engine_name) {
            Matrix2Fr points(4, 2);
            points << 0.0, 0.0,
                   1.0, 1.0,
                   1.0, 1.0,
                   0.0, 1.0;
            Matrix2Ir segments(4, 2);
            segments << 0, 1,
                     1, 2,
                     2, 3,
                     3, 0;

            Triangulation::Ptr t = Triangulation::create(engine_name);
            t->set_points(points);
            t->set_segments(segments);
            t->run();

            MatrixFr vertices = t->get_vertices();
            MatrixIr faces = t->get_faces();
            assert_valid_triangulation(vertices, faces);
            ASSERT_LE(3, vertices.rows());
            ASSERT_LE(1, faces.rows());
        }

};
