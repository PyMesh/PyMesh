/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>

#include <Core/Exception.h>

#include <Assembler/FESetting/FESettingFactory.h>
#include <Assembler/FESetting/FESetting.h>

#include <TestBase.h>

class FESettingTest : public TestBase {
    protected:
        typedef FESetting::FEMeshPtr FEMeshPtr;
        typedef FESetting::Ptr FESettingPtr;

        virtual MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshFactory()
                .load_file(mesh_file)
                .with_connectivity("all")
                .with_attribute("face_normal")
                .with_attribute("vertex_normal")
                .with_attribute("face_area")
                .with_attribute("voxel_volume")
                .create_shared();
        }
};

TEST_F(FESettingTest, Creation) {
    MeshPtr mesh = load_mesh("cube.msh");
    FESettingPtr setting = FESettingFactory(mesh)
        .with_material("test_material")
        .create();

    ASSERT_EQ(3, setting->get_mesh()->getDim());
}

TEST_F(FESettingTest, MissingMaterial) {
    MeshPtr mesh = load_mesh("cube.msh");
    ASSERT_THROW(FESettingFactory(mesh).create(), RuntimeError);
}

