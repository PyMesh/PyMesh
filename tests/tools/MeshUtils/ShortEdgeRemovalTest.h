#pragma once
#include <algorithm>
#include <string>
#include <tr1/memory>

#include <MeshUtils/ShortEdgeRemoval.h>

class ShortEdgeRemovalTest : public ::testing::Test {
    protected:
        void check_num_faces_left(const ShortEdgeRemoval& remover, size_t n) {
            MatrixIr faces_left = remover.get_faces();
            ASSERT_EQ(n, faces_left.rows());
        }

        void check_face_validity(const ShortEdgeRemoval& remover) {
            MatrixFr vertices_left = remover.get_vertices();
            MatrixIr faces_left = remover.get_faces();
            const size_t num_vertices = vertices_left.rows();
            const size_t num_faces = faces_left.rows();
            const size_t vertex_per_face = faces_left.cols();
            for (size_t i=0; i<faces_left.size(); i++) {
                ASSERT_LT(faces_left.data()[i], num_vertices);
            }
        }

        /**
         * Assert that v is either one of vertices left unchanged or is within
         * threshold away from a vertex after remove short edges.
         */
        void check_preserved_vertex(const ShortEdgeRemoval& remover,
                const VectorF& target, Float threshold) {
            MatrixFr vertices_left = remover.get_vertices();
            MatrixIr faces_left = remover.get_faces();
            const size_t num_vertices = vertices_left.rows();
            const size_t num_faces = faces_left.rows();
            const size_t vertex_per_face = faces_left.cols();

            Float min_dist = 1.0 + threshold;
            for (size_t i=0; i<num_faces; i++) {
                for (size_t j=0; j<vertex_per_face; j++) {
                    const VectorF& vertex = vertices_left.row(faces_left(i,j));
                    Float dist = (target - vertex).norm();
                    min_dist = std::min(min_dist, dist);
                }
            }

            ASSERT_LT(min_dist, threshold);
        }
};

TEST_F(ShortEdgeRemovalTest, SingleLargeFace) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,2;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.2);

    check_num_faces_left(remover, 1);
    check_face_validity(remover);
}

TEST_F(ShortEdgeRemovalTest, DegeneratedFace) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,1;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.2);

    check_num_faces_left(remover, 0);
    check_face_validity(remover);
}

TEST_F(ShortEdgeRemovalTest, ZeroThreshold) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,2;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.0);

    check_num_faces_left(remover, 0);
    check_face_validity(remover);
}

TEST_F(ShortEdgeRemovalTest, SingleFace) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.1, 0.1, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,2;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.2);

    check_num_faces_left(remover, 0);
    check_face_validity(remover);
}

TEST_F(ShortEdgeRemovalTest, DoubleFace) {
    MatrixFr vertices(4,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.1, 0.1, 0.0,
        0.0, 1.0, 0.0;
    MatrixIr faces(2,3);
    faces << 0,1,2,
             0,1,3;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.2);

    check_num_faces_left(remover, 1);
    check_face_validity(remover);
}

TEST_F(ShortEdgeRemovalTest, SingleTinyFace) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        0.1, 0.0, 0.0,
        0.0, 0.1, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,2;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.2);

    check_num_faces_left(remover, 0);
    check_face_validity(remover);
}

TEST_F(ShortEdgeRemovalTest, DoubleTinyFace) {
    MatrixFr vertices(4,3);
    vertices <<
        0.0, 0.0, 0.0,
        0.1, 0.0, 0.0,
        0.0, 0.1, 0.0,
        0.0, 0.0, 0.1;
    MatrixIr faces(2,3);
    faces << 0,1,2,
             0,1,3;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.2);

    check_num_faces_left(remover, 0);
    check_face_validity(remover);
}

TEST_F(ShortEdgeRemovalTest, TinyEdgeChain) {
    // Divid the edge (0,0,0), (1,0,0) into N segments.
    // Form a triangle fan with vertex (0,1,0).
    const size_t N = 100;
    const Float threshold = 0.2;
    Vector3F v0(0.0, 0.0, 0.0);
    Vector3F v1(1.0, 0.0, 0.0);
    Vector3F v2(0.0, 1.0, 0.0);
    MatrixFr vertices(N+2, 3);
    vertices.row(0) = v2;
    for (size_t i=0; i<=N; i++) {
        Float ratio = Float(i) / Float(N);
        vertices.row(i+1) = (1.0 - ratio) * v0 + ratio * v1;
    }

    MatrixIr faces(N, 3);
    for (size_t i=0; i<N; i++) {
        faces.row(i) = Vector3I(0, i+1, i+2);
    }

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(threshold);

    check_face_validity(remover);
    check_preserved_vertex(remover, v0, threshold);
    check_preserved_vertex(remover, v1, threshold);
    check_preserved_vertex(remover, v2, threshold);
}

