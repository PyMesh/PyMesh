/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <Mesh.h>

#include <TestBase.h>

class WriterTest : public TestBase {
    protected:
        virtual void SetUp() {
            TestBase::SetUp();
            m_tmp_dir = "/tmp/";
        }

        MeshPtr load_tmp_mesh(const std::string& mesh_file) {
            MeshPtr mesh = MeshFactory()
                .load_file(m_tmp_dir + mesh_file)
                .drop_zero_dim()
                .create_shared();
            return mesh;
        }

        void write_tmp_mesh(const std::string& mesh_file, MeshPtr mesh,
                bool in_ascii=false) {
            write_mesh(m_tmp_dir + mesh_file, mesh, in_ascii);
        }

        void write_tmp_mesh_raw(const std::string& mesh_file,
                VectorF& vertices,
                VectorI& faces,
                VectorI& voxels,
                size_t dim,
                size_t vertex_per_face,
                size_t vertex_per_voxel,
                bool in_ascii=false) {
            write_mesh_raw(m_tmp_dir + mesh_file,
                    vertices, faces, voxels,
                    dim, vertex_per_face, vertex_per_voxel, in_ascii);
        }

        void remove(const std::string& filename) {
            std::string full_name = m_tmp_dir + filename;
            int r = std::remove(full_name.c_str());
            ASSERT_EQ(0, r);
        }

        void assert_eq_vertices(MeshPtr mesh1, MeshPtr mesh2) {
            ASSERT_EQ(mesh1->get_dim(), mesh2->get_dim());
            const VectorF& vertices1 = mesh1->get_vertices();
            const VectorF& vertices2 = mesh2->get_vertices();
            ASSERT_EQ(vertices1.size(), vertices2.size());
            if (vertices1.size() > 0) {
                VectorF diff = vertices1 - vertices2;
                ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
                ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
            }
        }

        void assert_eq_faces(MeshPtr mesh1, MeshPtr mesh2) {
            const VectorI& faces1 = mesh1->get_faces();
            const VectorI& faces2 = mesh2->get_faces();
            ASSERT_EQ(faces1.size(), faces2.size());
            if (faces1.size() > 0) {
                VectorI diff = faces1 - faces2;
                ASSERT_EQ(0, diff.minCoeff());
                ASSERT_EQ(0, diff.maxCoeff());
            }
        }

        void assert_eq_voxels(MeshPtr mesh1, MeshPtr mesh2) {
            const VectorI& voxels1 = mesh1->get_voxels();
            const VectorI& voxels2 = mesh2->get_voxels();
            ASSERT_EQ(voxels1.size(), voxels2.size());
            if (voxels1.size() > 0) {
                VectorI diff = voxels1 - voxels2;
                ASSERT_EQ(0, diff.minCoeff());
                ASSERT_EQ(0, diff.maxCoeff());
            }
        }

        void assert_eq_attribute(MeshPtr mesh1, MeshPtr mesh2,
                const std::string& attr_name) {
            const VectorF& attr1 = mesh1->get_attribute(attr_name);
            const VectorF& attr2 = mesh2->get_attribute(attr_name);
            ASSERT_EQ(attr1.size(), attr2.size());
            VectorF diff = attr1 - attr2;
            ASSERT_NEAR(0.0, diff.minCoeff(), 1e-6);
            ASSERT_NEAR(0.0, diff.maxCoeff(), 1e-6);
        }

        void assert_eq_voxel_tensor_attribute(MeshPtr mesh1, MeshPtr mesh2,
                const std::string& attr_name) {
            const size_t num_voxels = mesh1->get_num_voxels();
            const VectorF& attr1 = mesh1->get_attribute(attr_name);
            const VectorF& attr2 = mesh2->get_attribute(attr_name);

            ASSERT_EQ(0, attr1.size() % num_voxels);
            ASSERT_EQ(0, attr2.size() % num_voxels);
            const size_t tensor_size1 = attr1.size() / num_voxels;
            const size_t tensor_size2 = attr2.size() / num_voxels;

            for (size_t i=0; i<num_voxels; i++) {
                const VectorF& val1 = attr1.segment(i*tensor_size1, tensor_size1);
                const VectorF& val2 = attr2.segment(i*tensor_size2, tensor_size2);
                MatrixF tensor1 = unflatten_tensor(val1);
                MatrixF tensor2 = unflatten_tensor(val2);
                MatrixF diff = tensor1 - tensor2;
                ASSERT_NEAR(0.0, diff.minCoeff(), 1e-6);
                ASSERT_NEAR(0.0, diff.maxCoeff(), 1e-6);
            }
        }

        MatrixF unflatten_tensor(const VectorF& flattened_tensor) {
            const size_t size = flattened_tensor.size();
            const VectorF& A = flattened_tensor;
            MatrixF tensor(3, 3);
            if (size == 3) {
                tensor << A[0], A[2], 0.0,
                          A[2], A[1], 0.0,
                           0.0,  0.0, 0.0;
            } else if (size == 6) {
                tensor << A[0], A[5], A[4],
                          A[5], A[1], A[3],
                          A[4], A[3], A[2];
            } else if (size == 9) {
                tensor << A[0], A[3], A[6],
                          A[1], A[4], A[7],
                          A[2], A[5], A[8];
            } else {
                std::stringstream err_msg;
                err_msg << "Invalid flattened tensor size: " << size;
                throw RuntimeError(err_msg.str());
            }
            return tensor;
        }

        MeshPtr write_and_load(const std::string& filename, MeshPtr mesh,
                bool in_ascii=false) {
            write_tmp_mesh(filename, mesh, in_ascii);
            MeshPtr r = load_tmp_mesh(filename);
            remove(filename);
            return r;
        }

        MeshPtr write_and_load_raw(const std::string& filename, MeshPtr mesh) {
            write_tmp_mesh_raw(
                    filename,
                    mesh->get_vertices(),
                    mesh->get_faces(),
                    mesh->get_voxels(),
                    mesh->get_dim(),
                    mesh->get_vertex_per_face(),
                    mesh->get_vertex_per_voxel());
            return load_tmp_mesh(filename);
        }

    protected:
        std::string m_tmp_dir;
};
