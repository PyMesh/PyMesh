/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Boolean/Clipper/ClipperEngine.h>
#include "../BooleanEngineTest.h"

class ClipperEngineTest : public BooleanEngineTest {
    protected:
        BooleanPtr get_disjoint_setting(MeshPtr mesh) {
            BooleanPtr clipper_engine = BooleanEngine::create("clipper");
            const size_t num_vertices = mesh->get_num_vertices();

            MatrixFr vertices_1 = extract_vertices(mesh);
            MatrixIr faces_1    = extract_faces(mesh);

            VectorF bbox_min = vertices_1.colwise().minCoeff();
            VectorF bbox_max = vertices_1.colwise().maxCoeff();
            VectorF bbox_size = bbox_max - bbox_min;

            MatrixFr vertices_2 = vertices_1;
            translate(vertices_2, bbox_size * 2);

            clipper_engine->set_mesh_1(vertices_1, faces_1);
            clipper_engine->set_mesh_2(vertices_2, faces_1);
            return clipper_engine;
        }

        BooleanPtr get_overlap_setting(MeshPtr mesh) {
            BooleanPtr clipper_engine = BooleanEngine::create("clipper");
            const size_t num_vertices = mesh->get_num_vertices();

            MatrixFr vertices_1 = extract_vertices(mesh);
            MatrixIr faces_1    = extract_faces(mesh);

            VectorF bbox_min = vertices_1.colwise().minCoeff();
            VectorF bbox_max = vertices_1.colwise().maxCoeff();
            VectorF bbox_size = bbox_max - bbox_min;

            MatrixFr vertices_2 = vertices_1;
            translate(vertices_2, bbox_size * 0.5);

            clipper_engine->set_mesh_1(vertices_1, faces_1);
            clipper_engine->set_mesh_2(vertices_2, faces_1);
            return clipper_engine;
        }

        BooleanPtr get_epsilon_box_setting(
                const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p, const Float EPS=0.1) {
            MatrixFr box_vertices;
            MatrixIr box_faces;
            generate_epsilon_box(p, box_vertices, box_faces, EPS);

            BooleanPtr clipper_engine = BooleanEngine::create("clipper");
            clipper_engine->set_mesh_1(vertices, faces);
            clipper_engine->set_mesh_2(box_vertices, box_faces);
            clipper_engine->compute_intersection();
            return clipper_engine;
        }

        void assert_interior(const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p) {
            const Float EPS = 0.1;
            BooleanPtr clipper_engine = get_epsilon_box_setting(vertices, faces, p, EPS);

            const MatrixFr& vts = clipper_engine->get_vertices();
            VectorF bbox_max = vts.colwise().maxCoeff();
            VectorF bbox_min = vts.colwise().minCoeff();
            VectorF bbox_size = bbox_max - bbox_min;

            ASSERT_FLOAT_EQ(EPS*2, bbox_size.maxCoeff());
            ASSERT_FLOAT_EQ(EPS*2, bbox_size.minCoeff());
        }

        void assert_on_boundary(const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p) {
            const Float EPS = 0.1;
            BooleanPtr clipper_engine = get_epsilon_box_setting(vertices, faces, p, EPS);

            const MatrixFr& vts = clipper_engine->get_vertices();
            VectorF bbox_max = vts.colwise().maxCoeff();
            VectorF bbox_min = vts.colwise().minCoeff();
            VectorF bbox_size = bbox_max - bbox_min;

            // The intersection of epsilon box with the given shape would
            // 1. cut off one or more corners of the epsilon box, in which case
            //    the number of verticing should be > 4.
            // 2. if the number of vertices <= 4, then the bbox of the
            //    intersection must be smaller than the epsilon box.
            ASSERT_TRUE(vts.rows() > 4 || EPS*2 > bbox_size.minCoeff());
        }

        void assert_exterior(const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p) {
            const Float EPS = 0.1;
            BooleanPtr clipper_engine = get_epsilon_box_setting(vertices, faces, p, EPS);
            ASSERT_EQ(0, clipper_engine->get_vertices().rows());
        }

        void generate_epsilon_box(const VectorF& p,
                MatrixFr& vertices, MatrixIr& faces, Float epsilon) {
            const size_t dim = p.size();
            if (dim == 2) {
                vertices.resize(4, dim);
                vertices << p[0]-epsilon, p[1]-epsilon,
                            p[0]+epsilon, p[1]-epsilon,
                            p[0]+epsilon, p[1]+epsilon,
                            p[0]-epsilon, p[1]+epsilon;

                faces.resize(2, 3);
                faces << 0, 1, 2,
                         0, 2, 3;
            } else {
                throw NotImplementedError("Only 2D epsilon box is supported.");
            }
        }
};

TEST_F(ClipperEngineTest, disjoint_union) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_disjoint_setting(mesh);
    clipper_engine->compute_union();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    ASSERT_EQ(num_vertices * 2, vertices.rows());
    ASSERT_EQ(num_faces * 2, faces.rows());
}

TEST_F(ClipperEngineTest, disjoint_intersection) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_disjoint_setting(mesh);
    clipper_engine->compute_intersection();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    ASSERT_EQ(0, vertices.rows());
    ASSERT_EQ(0, faces.rows());
}

TEST_F(ClipperEngineTest, disjoint_difference) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_disjoint_setting(mesh);
    clipper_engine->compute_difference();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    ASSERT_EQ(num_vertices, vertices.rows());
    ASSERT_EQ(num_faces, faces.rows());
}

TEST_F(ClipperEngineTest, disjoint_symmetric_difference) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_disjoint_setting(mesh);
    clipper_engine->compute_symmetric_difference();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    ASSERT_EQ(num_vertices * 2, vertices.rows());
    ASSERT_EQ(num_faces * 2, faces.rows());
}

TEST_F(ClipperEngineTest, overlap_union) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_overlap_setting(mesh);
    clipper_engine->compute_union();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();

    VectorF origin = VectorF::Zero(2);
    VectorF corner = VectorF::Ones(2);
    assert_interior(vertices, faces, origin);
    assert_interior(vertices, faces, corner);
    assert_on_boundary(vertices, faces, -corner);
}

TEST_F(ClipperEngineTest, overlap_intersection) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_overlap_setting(mesh);
    clipper_engine->compute_intersection();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();

    VectorF origin = VectorF::Zero(2);
    VectorF corner = VectorF::Ones(2);
    assert_on_boundary(vertices, faces, origin);
    assert_on_boundary(vertices, faces, corner);
    assert_exterior(vertices, faces, -corner);
}

TEST_F(ClipperEngineTest, overlap_difference) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_overlap_setting(mesh);
    clipper_engine->compute_difference();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    VectorF origin = VectorF::Zero(2);
    VectorF corner = VectorF::Ones(2);
    assert_on_boundary(vertices, faces, origin);
    assert_exterior(vertices, faces, corner);
    assert_on_boundary(vertices, faces, -corner);
}

TEST_F(ClipperEngineTest, overlap_symmetric_difference) {
    MeshPtr mesh = load_mesh("square_2D.obj");

    BooleanPtr clipper_engine = get_overlap_setting(mesh);
    clipper_engine->compute_symmetric_difference();

    const MatrixFr& vertices = clipper_engine->get_vertices();
    const MatrixIr& faces = clipper_engine->get_faces();

    VectorF origin = VectorF::Zero(2);
    VectorF corner = VectorF::Ones(2);
    assert_on_boundary(vertices, faces, origin);
    assert_on_boundary(vertices, faces, corner);
    assert_on_boundary(vertices, faces, -corner);
}

