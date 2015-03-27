/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <TestBase.h>

class VertexVolumeAttributeTest : public TestBase {
    protected:
        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("vertex_volume");
            return r;
        }
};

TEST_F(VertexVolumeAttributeTest, tet) {
    MeshPtr tet = load_mesh("tet.msh");
    ASSERT_TRUE(tet->has_attribute("vertex_volume"));
    VectorF volumes = tet->get_attribute("vertex_volume");
    ASSERT_TRUE((volumes.array() == volumes[0]).all());

    tet->add_attribute("voxel_volume");
    Float total_volume = tet->get_attribute("voxel_volume").sum();
    ASSERT_FLOAT_EQ(total_volume, volumes.sum());
}

TEST_F(VertexVolumeAttributeTest, hex) {
    MeshPtr hex = load_mesh("hex.msh");
    ASSERT_TRUE(hex->has_attribute("vertex_volume"));
    VectorF volumes = hex->get_attribute("vertex_volume");
    ASSERT_FLOAT_EQ(1.0, volumes[0]);
    ASSERT_TRUE((volumes.array() == volumes[0]).all());

    hex->add_attribute("voxel_volume");
    Float total_volume = hex->get_attribute("voxel_volume").sum();
    ASSERT_FLOAT_EQ(total_volume, volumes.sum());
}

TEST_F(VertexVolumeAttributeTest, surface_mesh) {
    MeshPtr mesh = load_mesh("cube.obj");
    ASSERT_TRUE(mesh->has_attribute("vertex_volume"));
    VectorF volumes = mesh->get_attribute("vertex_volume");
    ASSERT_EQ(0, volumes.size());
}

TEST_F(VertexVolumeAttributeTest, 2D_mesh) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    ASSERT_TRUE(mesh->has_attribute("vertex_volume"));
    VectorF volumes = mesh->get_attribute("vertex_volume");
    ASSERT_EQ(0, volumes.size());
}
