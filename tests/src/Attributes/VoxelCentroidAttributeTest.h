#pragma once

#include <string>

#include <Misc/Environment.h>

class VoxelCentroidAttributeTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string proj_root =
                Environment::get_required("PYMESH_PATH");
            m_data_dir = proj_root + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .with_attribute("voxel_centroid")
                    .create());
        }

        void validate_centroid(MeshPtr mesh, const VectorF& centroids) {
            const size_t dim = mesh->get_dim();
            const size_t num_voxels = mesh->get_num_voxels();
            const size_t num_vertex_per_voxel = mesh->get_vertex_per_voxel();
            for (size_t i=0; i<num_voxels; i++) {
                VectorI voxel = mesh->get_voxel(i);
                VectorF centroid = centroids.segment(i*dim, dim);
                VectorF vertex_sum = VectorF::Zero(dim);
                for (size_t j=0; j<num_vertex_per_voxel; j++) {
                    vertex_sum += mesh->get_vertex(voxel[j]);
                }
                Float diff = (centroid * num_vertex_per_voxel - vertex_sum).norm();
                ASSERT_FLOAT_EQ(0.0, diff);
            }
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VoxelCentroidAttributeTest, Cube) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    ASSERT_TRUE(cube_tet->has_attribute("voxel_centroid"));
    VectorF voxel_centroids = cube_tet->get_attribute("voxel_centroid");
    ASSERT_EQ(cube_tet->get_num_voxels() * cube_tet->get_dim(), voxel_centroids.size());
    validate_centroid(cube_tet, voxel_centroids);
}

TEST_F(VoxelCentroidAttributeTest, Hex) {
    MeshPtr hex = load_mesh("hex.msh");
    ASSERT_TRUE(hex->has_attribute("voxel_centroid"));
    VectorF voxel_centroids = hex->get_attribute("voxel_centroid");
    ASSERT_EQ(hex->get_num_voxels() * hex->get_dim(), voxel_centroids.size());
    validate_centroid(hex, voxel_centroids);
}

