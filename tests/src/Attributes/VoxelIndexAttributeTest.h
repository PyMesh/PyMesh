#pragma once

#include <string>

#include <Misc/Environment.h>

class VoxelIndexAttributeTest : public ::testing::Test {
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
                    .with_attribute("voxel_index")
                    .create());
        }

    protected:
        std::string m_data_dir;
};

TEST_F(VoxelIndexAttributeTest, cube) {
    MeshPtr cube = load_mesh("cube.msh");
    const size_t num_voxels = cube->get_num_voxels();
    const VectorF& indices = cube->get_attribute("voxel_index");
    for (size_t i=0; i<num_voxels; i++) {
        ASSERT_FLOAT_EQ(Float(i), indices[i]);
    }
}
