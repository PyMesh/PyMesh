#pragma once

#include <string>

#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <Core/Exception.h>

#include <Assembler/FESetting/FESettingFactory.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/Assemblers/Assembler.h>

class AssemblerTest : public ::testing::Test {
    protected:
        typedef FESetting::MeshPtr MeshPtr;
        typedef FESetting::FEMeshPtr FEMeshPtr;
        typedef FESetting::Ptr FESettingPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
            init_assembler();
        }

        virtual void init_assembler()=0;

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
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
