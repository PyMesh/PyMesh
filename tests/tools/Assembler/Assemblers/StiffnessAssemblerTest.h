#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <Core/Exception.h>

#include <Assembler/FESetting/FESettingFactory.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/Assemblers/Assembler.h>
#include <Assembler/Assemblers/StiffnessAssembler.h>

class StiffnessAssemblerTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;
        typedef FEMeshAdaptor::Ptr FEMeshPtr;
        typedef FESetting::Ptr FESettingPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
            m_assembler = Assembler::Ptr(new StiffnessAssembler());
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .with_connectivity("all")
                    .with_attribute("face_normal")
                    .with_attribute("vertex_normal")
                    .with_attribute("face_area")
                    .with_attribute("voxel_volume")
                    .create());
        }

        FESettingPtr load_setting(const std::string& filename) {
            MeshPtr mesh = load_mesh(filename);
            return FESettingFactory(mesh)
                .with_material("TestMaterial")
                .create();
        }

    protected:
        std::string m_data_dir;
        Assembler::Ptr m_assembler;
};

TEST_F(StiffnessAssemblerTest, Size) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix K = m_assembler->assemble(setting);
    ASSERT_EQ(12, K.rows());
    ASSERT_EQ(12, K.cols());
}

TEST_F(StiffnessAssemblerTest, Entries) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix K = m_assembler->assemble(setting);
    ASSERT_FLOAT_EQ(0.144337722129181, K.coeff(0, 0));
    ASSERT_FLOAT_EQ(-0.048112608063777, K.coeff(3, 0));
    ASSERT_FLOAT_EQ(0.240562529208718, K.coeff(11, 11));
}

TEST_F(StiffnessAssemblerTest, FrobeniousNorm) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix K = m_assembler->assemble(setting);
    ASSERT_FLOAT_EQ(0.942809221566362, K.norm());
}

