/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <string>

#include <MeshUtils/ShortEdgeRemoval.h>

#include <TestBase.h>

class ShortEdgeRemovalTest : public TestBase {
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

        void check_face_indices(const ShortEdgeRemoval& remover,
                const MatrixFr& ori_vertices, const MatrixIr& ori_faces, Float threshold) {
            const size_t dim = ori_vertices.cols();
            MatrixFr cur_vertices = remover.get_vertices();
            MatrixIr cur_faces = remover.get_faces();
            VectorI face_indices = remover.get_face_indices();
            const size_t num_faces = cur_faces.rows();
            ASSERT_EQ(num_faces, face_indices.size());

            for (size_t i=0; i<num_faces; i++) {
                VectorI cur_face = cur_faces.row(i);
                VectorI ori_face = ori_faces.row(face_indices[i]);
                ASSERT_EQ(dim, cur_face.size());
                ASSERT_EQ(dim, ori_face.size());
                VectorF cur_centroid = VectorF::Zero(dim);
                VectorF ori_centroid = VectorF::Zero(dim);
                for (size_t j=0; j<dim; j++) {
                    ASSERT_LT(cur_face[j], cur_vertices.rows());
                    ASSERT_LT(ori_face[j], ori_vertices.rows());
                    cur_centroid += cur_vertices.row(cur_face[j]);
                    ori_centroid += ori_vertices.row(ori_face[j]);
                }
                cur_centroid /= dim;
                ori_centroid /= dim;
                ASSERT_LT((cur_centroid - ori_centroid).norm(), threshold);
            }
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
    check_face_indices(remover, vertices, faces, 0.2);
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
    check_face_indices(remover, vertices, faces, 0.2);
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
    check_face_indices(remover, vertices, faces, 0.0);
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
    check_face_indices(remover, vertices, faces, 0.2);
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
    check_face_indices(remover, vertices, faces, 0.2);
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
    check_face_indices(remover, vertices, faces, 0.2);
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
    check_face_indices(remover, vertices, faces, 0.2);
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
    check_face_indices(remover, vertices, faces, threshold);
}

TEST_F(ShortEdgeRemovalTest, FaceIndices) {
    MatrixFr vertices(4,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.1;
    MatrixIr faces(2,3);
    faces << 0,1,2,
             0,2,3;

    ShortEdgeRemoval remover(vertices, faces);
    remover.run(0.2);

    check_face_validity(remover);
    check_face_indices(remover, vertices, faces, 0.2);
}

TEST_F(ShortEdgeRemovalTest, importance) {
    MatrixFr vertices(4,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.1;
    MatrixIr faces(2,3);
    faces << 0,1,2,
             0,2,3;
    VectorI importance(4);
    importance << 0, 0, 0, 1;

    ShortEdgeRemoval remover(vertices, faces);
    remover.set_importance(importance);
    remover.run(0.2);

    check_face_validity(remover);
    check_face_indices(remover, vertices, faces, 0.2);
    check_preserved_vertex(remover, vertices.row(3), 1e-12);
}

TEST_F(ShortEdgeRemovalTest, importance2) {
    MatrixFr vertices(4,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.1;
    MatrixIr faces(2,3);
    faces << 0,1,2,
             0,2,3;
    VectorI importance(4);
    importance << 0, 0, 1, 0;

    ShortEdgeRemoval remover(vertices, faces);
    remover.set_importance(importance);
    remover.run(0.2);

    check_face_validity(remover);
    check_face_indices(remover, vertices, faces, 0.2);
    check_preserved_vertex(remover, vertices.row(2), 1e-12);
}

TEST_F(ShortEdgeRemovalTest, ChainOfImportance) {
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
    VectorI importance = VectorI::Zero(N+2);
    importance[0] = -1;
    importance[1] = -1;
    importance[N+1] = -1;

    ShortEdgeRemoval remover(vertices, faces);
    remover.set_importance(importance);
    remover.run(threshold);

    check_face_validity(remover);
    check_preserved_vertex(remover, v0, 1e-12);
    check_preserved_vertex(remover, v1, 1e-12);
    check_preserved_vertex(remover, v2, 1e-12);
    check_face_indices(remover, vertices, faces, threshold);
}

