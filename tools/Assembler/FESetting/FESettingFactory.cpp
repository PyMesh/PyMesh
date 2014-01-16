#include "FESettingFactory.h"

#include <sstream>
#include <iostream>

#include <Core/Exception.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/Mesh/FEMeshFactory.h>
#include <Assembler/ShapeFunctions/Integrator.h>
#include <Assembler/Materials/Material.h>

FESettingFactory::FESettingFactory(FESettingFactory::MeshPtr mesh) {
    m_mesh = FEMeshFactory::adapt(mesh);
    m_integrator = Integrator::create(m_mesh);
}

FESettingFactory& FESettingFactory::with_material(
        const std::string& material_name) {
    size_t dim = m_mesh->getDim();
    if (material_name == "TestMaterial") {
        m_material = Material::create_isotropic(dim, 1.0, 1.0, 0.0);
    } else {
        std::stringstream err_msg;
        err_msg << "Material " << material_name << " is not supported.";
        throw NotImplementedError(err_msg.str());
    }
    return *this;
}

FESettingFactory::FESettingPtr FESettingFactory::create() {
    if (!m_mesh) {
        throw RuntimeError("Mesh is not initialized");
    }
    if (!m_integrator) {
        throw RuntimeError("Integrator is not initialized");
    }
    if (!m_material) {
        throw RuntimeError("Material is not initialized");
    }

    return FESettingPtr(new FESetting(m_mesh, m_integrator, m_material));
}
