/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>
#include <functional>

#include <TestBase.h>
#include <Math/MatrixUtils.h>
#include <MeshUtils/SubMesh.h>

class SubMeshTest : public TestBase {
    protected:
        VectorF compute_face_center(MeshPtr mesh, size_t i) {
            VectorI face = mesh->get_face(i);
            VectorF center = VectorF::Zero(mesh->get_dim());

            const size_t num_vertex_per_face = face.size();
            for (size_t i=0; i < num_vertex_per_face; i++) {
                center += mesh->get_vertex(face[i]);
            }
            center /= Float(num_vertex_per_face);
            return center;
        }

        VectorF compute_face_center(
                const MatrixFr& vertices,
                const MatrixIr& faces,
                size_t i) {
            const size_t dim = vertices.cols();
            const size_t num_vertex_per_face = faces.cols();
            VectorI face = faces.row(i);

            VectorF center = VectorF::Zero(dim);
            for (size_t i=0; i<num_vertex_per_face; i++) {
                center += vertices.row(face[i]);
            }
            center /= Float(num_vertex_per_face);
            return center;
        }

        void ASSERT_FACE_EQ(MeshPtr mesh_1, size_t face_1,
                const MatrixFr& vertices, const MatrixIr& faces,
                size_t face_2) {
            VectorF target_face_center = compute_face_center(mesh_1, face_1);
            VectorF result_face_center = compute_face_center(vertices, faces, face_2);
            ASSERT_FLOAT_EQ(0.0,
                    (result_face_center - target_face_center).squaredNorm());
        }
};

TEST_F(SubMeshTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    SubMesh::Ptr submesh = SubMesh::create(mesh);

    std::vector<size_t> selected_faces;
    selected_faces.push_back(0);

    submesh->filter_with_face_index(selected_faces);
    submesh->finalize();

    const MatrixFr& vertices = submesh->get_selected_vertices();
    const MatrixIr& faces = submesh->get_selected_faces();

    ASSERT_EQ(3, vertices.rows());
    ASSERT_EQ(1, faces.rows());

    ASSERT_FACE_EQ(mesh, 0, vertices, faces, 0);
}

TEST_F(SubMeshTest, 3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    SubMesh::Ptr submesh = SubMesh::create(mesh);

    std::vector<size_t> selected_faces;
    selected_faces.push_back(0);
    selected_faces.push_back(1);

    submesh->filter_with_face_index(selected_faces);
    submesh->finalize();

    const MatrixFr& vertices = submesh->get_selected_vertices();
    const MatrixIr& faces = submesh->get_selected_faces();

    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(2, faces.rows());

    ASSERT_FACE_EQ(mesh, 0, vertices, faces, 0);
    ASSERT_FACE_EQ(mesh, 1, vertices, faces, 1);
}

TEST_F(SubMeshTest, BoundaryFaces) {
    MeshPtr mesh = load_mesh("cube.msh");
    const size_t dim = mesh->get_dim();
    const size_t num_vertices = mesh->get_num_vertices();

    SubMesh::Ptr submesh = SubMesh::create(mesh);

    auto ori_vertices = MatrixUtils::reshape<MatrixFr>(
            mesh->get_vertices(), num_vertices, dim);
    VectorF bbox_min = ori_vertices.colwise().minCoeff();

    std::function<bool(VectorF)> filter([&](const VectorF& v) {
            return v[0] == bbox_min[0]; });
    submesh->filter_vertex_with_custom_function(filter);
    submesh->finalize();

    const MatrixFr& vertices = submesh->get_selected_vertices();
    const MatrixIr& faces = submesh->get_selected_faces();

    const size_t num_filted_vertices = vertices.rows();
    for (size_t i=0; i<num_filted_vertices; i++) {
        ASSERT_TRUE(filter(vertices.row(i)));
    }

    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(2, faces.rows());
}

TEST_F(SubMeshTest, NotFinalized) {
    MeshPtr mesh = load_mesh("cube.msh");
    SubMesh::Ptr submesh = SubMesh::create(mesh);
    ASSERT_THROW(submesh->get_selected_vertices(), RuntimeError);
    ASSERT_THROW(submesh->get_selected_faces(), RuntimeError);
    ASSERT_THROW(submesh->get_selected_vertex_indices(), RuntimeError);
    ASSERT_THROW(submesh->get_selected_face_indices(), RuntimeError);
}

