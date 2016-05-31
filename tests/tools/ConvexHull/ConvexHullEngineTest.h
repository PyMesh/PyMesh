/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>
#include <TestBase.h>

class ConvexHullEngineTest : public TestBase {
    protected:
        void ASSERT_SAME_BBOX(const MatrixFr& pts_1, const MatrixFr& pts_2) {
            VectorF bbox_min_1 = pts_1.colwise().minCoeff();
            VectorF bbox_max_1 = pts_1.colwise().maxCoeff();
            VectorF bbox_min_2 = pts_2.colwise().minCoeff();
            VectorF bbox_max_2 = pts_2.colwise().maxCoeff();

            ASSERT_FLOAT_EQ(0.0, (bbox_min_1 - bbox_min_2).norm());
            ASSERT_FLOAT_EQ(0.0, (bbox_max_1 - bbox_max_2).norm());
        }

        void ASSERT_INDEX_MAP_IS_VALID(const MatrixFr& pts,
                const MatrixFr& vertices, const VectorI& index_map) {
            const size_t num_vertices = vertices.rows();
            ASSERT_EQ(num_vertices, index_map.size());
            for (size_t i=0; i<num_vertices; i++) {
                const VectorF& ori_p = pts.row(index_map[i]);
                const VectorF& cur_p = vertices.row(i);
                ASSERT_FLOAT_EQ(0.0, (cur_p - ori_p).norm());
            }
        }

        void ASSERT_ORIENTATION_IS_VALID(
                const MatrixFr& vertices, const MatrixIr& faces) {
            const size_t dim = faces.cols();
            if (dim == 2) ASSERT_ORIENTATION_IS_VALID_2D(vertices, faces);
            else if (dim == 3) ASSERT_ORIENTATION_IS_VALID_3D(vertices, faces);
        }

        void ASSERT_ORIENTATION_IS_VALID_2D(
                const MatrixFr& vertices, const MatrixIr& faces) {
            VectorF center = 0.5 * (vertices.colwise().minCoeff() +
                    vertices.colwise().maxCoeff());
            const size_t num_faces = faces.rows();
            for (size_t i=0; i<num_faces; i++) {
                VectorF v0 = vertices.row(faces(i, 0)) - center.transpose();
                VectorF v1 = vertices.row(faces(i, 1)) - center.transpose();
                Float area = v0[0] * v1[1] - v0[1] * v1[0];
                ASSERT_GT(area, 0.0);
            }
        }

        void ASSERT_ORIENTATION_IS_VALID_3D(
                const MatrixFr& vertices, const MatrixIr& faces) {
            VectorF center = 0.5 * (vertices.colwise().minCoeff() +
                    vertices.colwise().maxCoeff());
            const size_t num_faces = faces.rows();
            for (size_t i=0; i<num_faces; i++) {
                Vector3F v0 = vertices.row(faces(i, 0)) - center.transpose();
                Vector3F v1 = vertices.row(faces(i, 1)) - center.transpose();
                Vector3F v2 = vertices.row(faces(i, 2)) - center.transpose();
                Float volume = v0.cross(v1).dot(v2);
                ASSERT_GT(volume, 0.0);
            }
        }
};
