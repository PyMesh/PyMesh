/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <MeshUtils/FinFaceRemoval.h>

#include <TestBase.h>

class FinFaceRemovalTest : public TestBase {
    protected:
        void check_face_index(const MatrixIr& in_faces, 
                const MatrixIr& out_faces, 
                const VectorI& face_indices) {
            const size_t num_output_faces = out_faces.rows();
            ASSERT_EQ(num_output_faces, face_indices.size());
            for (size_t i=0; i<num_output_faces; i++) {
                const VectorI& input_f = in_faces.row(face_indices[i]);
                const VectorI& output_f = out_faces.row(i);
                ASSERT_TRUE((input_f.array() == output_f.array()).all());
            }
        }
};

TEST_F(FinFaceRemovalTest, zero_fin) {
    MatrixF vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                1.0, 1.0, 0.0;
    MatrixI faces(2, 3);
    faces << 0, 1, 2,
             2, 1, 3;

    FinFaceRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI face_indices = remover.get_face_indices();

    ASSERT_EQ(2, result_faces.rows());
    ASSERT_EQ(2, face_indices.rows());
    ASSERT_FLOAT_EQ(0.0, (vertices - result_vertices).norm());
    ASSERT_FLOAT_EQ(0.0, (faces - result_faces).norm());
    check_face_index(faces, result_faces, face_indices);
}

TEST_F(FinFaceRemovalTest, one_fin) {
    MatrixF vertices(4, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                1.0, 1.0, 0.0;
    MatrixI faces(3, 3);
    faces << 0, 1, 2,
             2, 1, 0,
             2, 1, 3;

    FinFaceRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI face_indices = remover.get_face_indices();

    ASSERT_EQ(1, result_faces.rows());
    ASSERT_EQ(1, face_indices.rows());
    check_face_index(faces, result_faces, face_indices);
}


TEST_F(FinFaceRemovalTest, duplicated_face) {
    MatrixF vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;
    MatrixI faces(2, 3);
    faces << 0, 1, 2,
             0, 2, 1;

    FinFaceRemoval remover(vertices, faces);
    remover.run();

    MatrixFr result_vertices = remover.get_vertices();
    MatrixIr result_faces = remover.get_faces();
    VectorI face_indices = remover.get_face_indices();

    ASSERT_EQ(0, result_faces.rows());
    ASSERT_EQ(0, face_indices.rows());
    check_face_index(faces, result_faces, face_indices);
}

