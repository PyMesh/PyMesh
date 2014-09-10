#pragma once

#include <string>
#include <set>

#include <Core/Exception.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

#include <MeshUtils/Subdivision.h>

class SimpleSubdivisionTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;
        typedef Subdivision::Ptr SubDivPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(MeshFactory()
                    .load_file(mesh_file)
                    .with_attribute("face_area")
                    .create());
        }

        SubDivPtr create_subdivision() {
            return Subdivision::create("simple");
        }

        MatrixFr extract_vertices(MeshPtr mesh) {
            const size_t dim = mesh->get_dim();
            const size_t num_vertices = mesh->get_num_vertices();
            VectorF flattened_vertices = mesh->get_vertices();
            Eigen::Map<MatrixFr> vertices(flattened_vertices.data(), num_vertices, dim);
            return vertices;
        }

        MatrixIr extract_faces(MeshPtr mesh) {
            const size_t num_faces = mesh->get_num_faces();
            const size_t num_vertex_per_face = mesh->get_vertex_per_face();
            VectorI flattened_faces = mesh->get_faces();
            Eigen::Map<MatrixIr> faces(flattened_faces.data(), num_faces, num_vertex_per_face);
            return faces;
        }

        void assert_equal_centroid(const MatrixFr& pts1, const MatrixFr& pts2) {
            const size_t dim = pts1.cols();
            ASSERT_EQ(dim, pts2.cols());

            VectorF centroid1 = VectorF::Zero(dim);
            VectorF centroid2 = VectorF::Zero(dim);

            for (size_t i=0; i<pts1.rows(); i++) {
                VectorF p = pts1.row(i);
                centroid1 = centroid1 + p;
            }
            centroid1 = centroid1 / pts1.rows();

            for (size_t i=0; i<pts1.rows(); i++) {
                VectorF p = pts2.row(i);
                centroid2 = centroid2 + p;
            }
            centroid2 = centroid2 / pts2.rows();

            Float dist = (centroid1 - centroid2).norm();
            ASSERT_FLOAT_EQ(0.0, dist);
        }

    protected:
        std::string m_data_dir;
};

TEST_F(SimpleSubdivisionTest, creation) {
    SubDivPtr sub = create_subdivision();
}

TEST_F(SimpleSubdivisionTest, square) {
    MeshPtr square = load_mesh("square_2D.obj");
    MatrixFr vertices = extract_vertices(square);
    MatrixIr faces = extract_faces(square);
    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(2, faces.rows());

    SubDivPtr sub = create_subdivision();
    sub->subdivide(vertices, faces, 1);

    MatrixFr sub_vertices = sub->get_vertices();
    MatrixIr sub_faces = sub->get_faces();

    ASSERT_EQ(8, sub_faces.rows());
    ASSERT_EQ(9, sub_vertices.rows());
    assert_equal_centroid(vertices, sub_vertices);
}

TEST_F(SimpleSubdivisionTest, cube) {
    MeshPtr cube = load_mesh("cube.obj");
    MatrixFr vertices = extract_vertices(cube);
    MatrixIr faces = extract_faces(cube);
    ASSERT_EQ(8, vertices.rows());
    ASSERT_EQ(12, faces.rows());

    SubDivPtr sub = create_subdivision();
    sub->subdivide(vertices, faces, 1);

    MatrixFr sub_vertices = sub->get_vertices();
    MatrixIr sub_faces = sub->get_faces();

    ASSERT_EQ(26, sub_vertices.rows());
    ASSERT_EQ(48, sub_faces.rows());
    assert_equal_centroid(vertices, sub_vertices);
}

TEST_F(SimpleSubdivisionTest, face_index) {
    MeshPtr square = load_mesh("square_2D.obj");
    MatrixFr vertices = extract_vertices(square);
    MatrixIr faces = extract_faces(square);

    SubDivPtr sub = create_subdivision();
    sub->subdivide(vertices, faces, 1);

    MatrixIr sub_faces = sub->get_faces();
    VectorI face_indices = sub->get_face_indices();

    ASSERT_EQ(sub_faces.rows(), face_indices.size());
    ASSERT_EQ(0, face_indices.minCoeff());
    ASSERT_EQ(faces.rows()-1, face_indices.maxCoeff());
}
