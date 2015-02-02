#pragma once

#include <vector>

#include <MeshUtils/SubMesh.h>

class SubMeshTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(MeshFactory()
                    .load_file(mesh_file)
                    .create());
        }

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

    protected:
        std::string m_data_dir;
};

TEST_F(SubMeshTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    SubMesh::Ptr submesh = SubMesh::create(mesh);

    std::vector<size_t> selected_faces;
    selected_faces.push_back(0);

    submesh->filter_with_face_index(selected_faces);
    submesh->finalize();

    const MatrixFr& vertices = submesh->get_vertices();
    const MatrixIr& faces = submesh->get_faces();

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

    const MatrixFr& vertices = submesh->get_vertices();
    const MatrixIr& faces = submesh->get_faces();

    ASSERT_EQ(4, vertices.rows());
    ASSERT_EQ(2, faces.rows());

    ASSERT_FACE_EQ(mesh, 0, vertices, faces, 0);
    ASSERT_FACE_EQ(mesh, 1, vertices, faces, 1);
}
