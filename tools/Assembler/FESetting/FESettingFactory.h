/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include "FESetting.h"

namespace PyMesh {

class FESettingFactory {
    public:
        typedef FESetting::Ptr FESettingPtr;
        typedef FESetting::MeshPtr MeshPtr;
        typedef FESetting::FEMeshPtr FEMeshPtr;
        typedef FESetting::FEBasisPtr FEBasisPtr;
        typedef FESetting::MaterialPtr MaterialPtr;

        FESettingFactory(MeshPtr mesh);
        FESettingFactory& with_material(const std::string& material_name);
        FESettingFactory& with_material(MaterialPtr material);
        FESettingPtr create();

    private:
        FEMeshPtr m_mesh;
        FEBasisPtr m_basis;
        MaterialPtr m_material;
};

}
