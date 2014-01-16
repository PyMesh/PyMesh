#pragma once

#include <string>

#include "FESetting.h"

class FESettingFactory {
    public:
        typedef Mesh::Ptr MeshPtr;
        typedef FEMeshAdaptor::Ptr FEMeshPtr;
        typedef FESetting::Ptr FESettingPtr;
        typedef Integrator::Ptr IntegratorPtr;
        typedef Material::Ptr MaterialPtr;

        FESettingFactory(MeshPtr mesh);
        FESettingFactory& with_material(const std::string& material_name);
        FESettingPtr create();

    private:
        FEMeshPtr m_mesh;
        IntegratorPtr m_integrator;
        MaterialPtr m_material;
};
