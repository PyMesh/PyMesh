#pragma once

#include <string>

#include "FESetting.h"

class FESettingFactory {
    public:
        typedef FESetting::Ptr FESettingPtr;
        typedef FESetting::MeshPtr MeshPtr;
        typedef FESetting::FEMeshPtr FEMeshPtr;
        typedef FESetting::IntegratorPtr IntegratorPtr;
        typedef FESetting::MaterialPtr MaterialPtr;

        FESettingFactory(MeshPtr mesh);
        FESettingFactory& with_material(const std::string& material_name);
        FESettingPtr create();

    private:
        FEMeshPtr m_mesh;
        IntegratorPtr m_integrator;
        MaterialPtr m_material;
};
