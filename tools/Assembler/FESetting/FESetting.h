#pragma once

#include <tr1/memory>

#include <Mesh.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/ShapeFunctions/Integrator.h>
#include <Assembler/Materials/Material.h>

class FESetting {
    public:
        typedef std::tr1::shared_ptr<FESetting> Ptr;
        typedef Mesh::Ptr MeshPtr;
        typedef Elements::Ptr FEMeshPtr;
        //typedef FEMeshAdaptor::Ptr FEMeshPtr;
        typedef Integrator::Ptr IntegratorPtr;
        typedef Material::Ptr MaterialPtr;

    public:
        FESetting(FEMeshPtr mesh, IntegratorPtr integrator,
                MaterialPtr material) :
            m_mesh(mesh), m_integrator(integrator), m_material(material) {}

        FEMeshPtr get_mesh() { return m_mesh; }
        IntegratorPtr get_integrator() { return m_integrator; }
        MaterialPtr get_material() { return m_material; }

    protected:
        FEMeshPtr m_mesh;
        IntegratorPtr m_integrator;
        MaterialPtr m_material;
};
