#pragma once

#include <string>

#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <Core/Exception.h>

#include <Assembler/FESetting/FESettingFactory.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/Assemblers/Assembler.h>
#include <Assembler/Assemblers/MassAssembler.h>

class MassAssemblerTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;
        typedef FEMeshAdaptor::Ptr FEMeshPtr;
        typedef FESetting::Ptr FESettingPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
            m_assembler = Assembler::Ptr(new MassAssembler());
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
                .with_material("test_material")
                .create();
        }

    protected:
        std::string m_data_dir;
        Assembler::Ptr m_assembler;
};

TEST_F(MassAssemblerTest, TetSize) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix M = m_assembler->assemble(setting);
    ASSERT_EQ(4, M.rows());
    ASSERT_EQ(4, M.cols());
}

TEST_F(MassAssemblerTest, SquareSize) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix M = m_assembler->assemble(setting);
    ASSERT_EQ(4, M.rows());
    ASSERT_EQ(4, M.cols());
}

TEST_F(MassAssemblerTest, TetPositiveEntries) {
    FESettingPtr setting = load_setting("tet.msh");
    ZSparseMatrix M = m_assembler->assemble(setting);
    size_t nz_entries = M.get_value_size();
    ASSERT_GT(nz_entries, 0);
    const Float* values = M.valuePtr();
    for (size_t i=0; i<nz_entries; i++) {
        ASSERT_GT(values[i], 0.0);
    }
}

TEST_F(MassAssemblerTest, SquarePositiveEntries) {
    FESettingPtr setting = load_setting("square_2D.obj");
    ZSparseMatrix M = m_assembler->assemble(setting);
    size_t nz_entries = M.get_value_size();
    ASSERT_GT(nz_entries, 0);
    const Float* values = M.valuePtr();
    for (size_t i=0; i<nz_entries; i++) {
        ASSERT_GT(values[i], 0.0);
    }
}

