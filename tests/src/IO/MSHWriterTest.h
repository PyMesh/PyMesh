#pragma once
#include <string>
#include <memory>
#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>
#include <IO/MSHWriter.h>

class MSHWriterTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string proj_root =
                Environment::get_required("PYMESH_PATH");
            m_data_dir = proj_root + "/tests/data/";
            m_tmp_dir = "/tmp/";
        }

        MeshPtr load_mesh(const std::string& mesh_file) {
            MeshPtr mesh = MeshFactory()
                .load_file(m_data_dir + mesh_file)
                .create_shared();
            return mesh;
        }

        MeshPtr load_tmp_mesh(const std::string& mesh_file) {
            MeshPtr mesh = MeshFactory()
                .load_file(m_tmp_dir + mesh_file)
                .create_shared();
            return mesh;
        }

        void remove(const std::string& filename) {
            std::string full_name = m_tmp_dir + filename;
            int r = std::remove(full_name.c_str());
            ASSERT_EQ(0, r);
        }

        void assert_eq_vertices(MeshPtr mesh1, MeshPtr mesh2) {
            const VectorF& vertices1 = mesh1->get_vertices();
            const VectorF& vertices2 = mesh2->get_vertices();
            ASSERT_EQ(vertices1.size(), vertices2.size());
            VectorF diff = vertices1 - vertices2;
            ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
            ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
        }

        void assert_eq_faces(MeshPtr mesh1, MeshPtr mesh2) {
            const VectorI& faces1 = mesh1->get_faces();
            const VectorI& faces2 = mesh2->get_faces();
            ASSERT_EQ(faces1.size(), faces2.size());
            VectorI diff = faces1 - faces2;
            ASSERT_EQ(0, diff.minCoeff());
            ASSERT_EQ(0, diff.maxCoeff());
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
            ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
            ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
        }

        void assert_eq_voxel_tensor_attribute(MeshPtr mesh1, MeshPtr mesh2,
                const std::string& attr_name) {
            const size_t num_voxels = mesh1->get_num_voxels();
            const VectorF& attr1 = mesh1->get_attribute(attr_name);
            const VectorF& attr2 = mesh2->get_attribute(attr_name);

            for (size_t i=0; i<num_voxels; i++) {
                const VectorF& val1 = attr1.segment(i*6, 6);
                const VectorF& val2 = attr2.segment(i*9, 9);
                MatrixF tensor1(3, 3);
                MatrixF tensor2(3, 3);
                tensor1 << val1[0], val1[5], val1[4],
                           val1[5], val1[1], val1[3],
                           val1[4], val1[3], val1[2];
                tensor2 << val2[0], val2[1], val2[2],
                           val2[3], val2[4], val2[5],
                           val2[6], val2[7], val2[8];
                MatrixF diff = tensor1 - tensor2;
                ASSERT_FLOAT_EQ(0.0, diff.minCoeff());
                ASSERT_FLOAT_EQ(0.0, diff.maxCoeff());
            }
        }

    protected:
        std::string m_data_dir;
        std::string m_tmp_dir;
};

TEST_F(MSHWriterTest, Cube) {
    std::string filename = "cube.obj";
    MeshPtr mesh = load_mesh(filename);

    std::string tmp_name = "tmp_cube_surface.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);

    remove(tmp_name);
}

TEST_F(MSHWriterTest, Cube2) {
    std::string filename = "cube.msh";
    MeshPtr mesh = load_mesh(filename);

    std::string tmp_name = "tmp_cube_volume.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_vertices(mesh, mesh2);
    assert_eq_faces(mesh, mesh2);
    assert_eq_voxels(mesh, mesh2);

    remove(tmp_name);
}

TEST_F(MSHWriterTest, VertexAttributes) {
    std::string filename = "tet.msh";
    MeshPtr mesh = load_mesh(filename);

    mesh->add_attribute("vertex_index");
    mesh->add_attribute("vertex_normal");

    std::string tmp_name = "tmp_tet_vertex_attr.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("vertex_index");
    writer.with_attribute("vertex_normal");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "vertex_index");
    assert_eq_attribute(mesh, mesh2, "vertex_normal");

    remove(tmp_name);
}

TEST_F(MSHWriterTest, FaceAttributes) {
    std::string filename = "cube.obj";
    MeshPtr mesh = load_mesh(filename);

    mesh->add_attribute("face_index");
    mesh->add_attribute("face_normal");

    std::string tmp_name = "tmp_cube_face_attr.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("face_index");
    writer.with_attribute("face_normal");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "face_index");
    assert_eq_attribute(mesh, mesh2, "face_normal");

    remove(tmp_name);
}

TEST_F(MSHWriterTest, VoxelAttributes) {
    std::string filename = "cube.msh";
    MeshPtr mesh = load_mesh(filename);
    const size_t dim = mesh->get_dim();
    const size_t num_voxels = mesh->get_num_voxels();

    VectorF vector_field = VectorF::LinSpaced(num_voxels * dim, 0.0, 1.0);
    VectorF tensor_field = VectorF::LinSpaced(
            num_voxels * dim * (dim + 1) / 2, 0.0, 1.0);

    mesh->add_attribute("voxel_index");
    mesh->add_attribute("voxel_vector");
    mesh->set_attribute("voxel_vector", vector_field);
    mesh->add_attribute("voxel_tensor");
    mesh->set_attribute("voxel_tensor", tensor_field);

    std::string tmp_name = "tmp_cube_voxel_attr.msh";
    MSHWriter writer;
    writer.set_output_filename(m_tmp_dir + tmp_name);
    writer.with_attribute("voxel_index");
    writer.with_attribute("voxel_vector");
    writer.with_attribute("voxel_tensor");
    writer.write_mesh(*mesh);

    MeshPtr mesh2 = load_tmp_mesh(tmp_name);
    assert_eq_attribute(mesh, mesh2, "voxel_index");
    assert_eq_attribute(mesh, mesh2, "voxel_vector");
    assert_eq_voxel_tensor_attribute(mesh, mesh2, "voxel_tensor");

    remove(tmp_name);
}

