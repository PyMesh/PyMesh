/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FESettingFactory.h"

#include <sstream>
#include <iostream>

#include <Core/Exception.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/Mesh/FEMeshFactory.h>
#include <Assembler/ShapeFunctions/FEBasis.h>
#include <Assembler/Materials/Material.h>

using namespace PyMesh;

FESettingFactory::FESettingFactory(FESettingFactory::MeshPtr mesh) {
    m_mesh = Elements::adapt(mesh);
    m_basis = FEBasisPtr(new FEBasis(m_mesh));
}

FESettingFactory& FESettingFactory::with_material(
        const std::string& material_name) {
    size_t dim = m_mesh->getDim();
    if (material_name == "test_material") {
        m_material = Material::create_isotropic(dim, 1.0, 1.0, 0.0);
    } else if (material_name == "periodic_material") {
        MaterialPtr mat1 = Material::create_isotropic(dim, 1.0, 1.0, 0.0);
        MaterialPtr mat2 = Material::create_isotropic(dim, 1.0, 2.0, 0.0);
        VectorF axis = VectorF::Zero(dim);
        axis[0] = 1;
        m_material = Material::create_periodic(mat1, mat2, axis, 1.0, 0.5, 0.0);
    } else if (material_name == "homogenized_material") {
        if (dim == 3) {
            throw NotImplementedError("Homogenized material is not supported in 3D");
        }
        size_t tensor_size = dim * dim;
        MatrixF C(tensor_size, tensor_size);
        C << 4.0/3.0,     0.0,     0.0, 7.0/5.0,
             0.0    ,     0.0,     0.0,     0.0,
             0.0    ,     0.0,     0.0,     0.0,
             7.0/5.0,     0.0,     0.0, 3.0/2.0;
        m_material = Material::create(1.0, C);
    } else {
        std::stringstream err_msg;
        err_msg << "Material \"" << material_name << "\" is not supported.";
        throw NotImplementedError(err_msg.str());
    }
    return *this;
}

FESettingFactory& FESettingFactory::with_material(
        MaterialPtr material) {
    m_material = material;
    return *this;
}

FESettingFactory::FESettingPtr FESettingFactory::create() {
    if (!m_mesh) {
        throw RuntimeError("Mesh is not initialized");
    }
    if (!m_basis) {
        throw RuntimeError("Basis is not initialized");
    }
    if (!m_material) {
        throw RuntimeError("Material is not initialized");
    }

    return FESettingPtr(new FESetting(m_mesh, m_basis, m_material));
}
