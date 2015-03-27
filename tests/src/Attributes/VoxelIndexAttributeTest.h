/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelIndexAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("voxel_index");
            return r;
        }
};

TEST_F(VoxelIndexAttributeTest, cube) {
    MeshPtr cube = load_mesh("cube.msh");
    const size_t num_voxels = cube->get_num_voxels();
    const VectorF& indices = cube->get_attribute("voxel_index");
    for (size_t i=0; i<num_voxels; i++) {
        ASSERT_FLOAT_EQ(Float(i), indices[i]);
    }
}
