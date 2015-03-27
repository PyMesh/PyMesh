/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/Exception.h>
#include <TestBase.h>

#include <Assembler/FESetting/FESettingFactory.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/Assemblers/Assembler.h>

class AssemblerTest : public TestBase {
    protected:
        typedef FESetting::MeshPtr MeshPtr;
        typedef FESetting::FEMeshPtr FEMeshPtr;
        typedef FESetting::Ptr FESettingPtr;

        virtual void SetUp() {
            TestBase::SetUp();
            init_assembler();
        }

        virtual void init_assembler()=0;

        FESettingPtr load_setting(const std::string& filename) {
            MeshPtr mesh = load_mesh(filename);
            return FESettingFactory(mesh)
                .with_material("test_material")
                .create();
        }

    protected:
        Assembler::Ptr m_assembler;
};
