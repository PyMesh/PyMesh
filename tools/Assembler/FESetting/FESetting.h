/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>

#include <Mesh.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/ShapeFunctions/FEBasis.h>
#include <Assembler/Materials/Material.h>

namespace PyMesh {

class FESetting {
    public:
        typedef std::shared_ptr<FESetting> Ptr;
        typedef Mesh::Ptr MeshPtr;
        typedef Elements::Ptr FEMeshPtr;
        typedef FEBasis::Ptr FEBasisPtr;
        typedef Material::Ptr MaterialPtr;

    public:
        FESetting(FEMeshPtr mesh, FEBasisPtr basis, MaterialPtr material) :
            m_mesh(mesh), m_basis(basis), m_material(material) {}

        FEMeshPtr get_mesh() { return m_mesh; }
        FEBasisPtr get_basis() { return m_basis; }
        MaterialPtr get_material() { return m_material; }

        void set_material(MaterialPtr material) {
            m_material = material;
        }

    protected:
        FEMeshPtr m_mesh;
        FEBasisPtr m_basis;
        MaterialPtr m_material;
};

}
