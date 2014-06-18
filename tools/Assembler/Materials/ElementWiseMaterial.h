#pragma once

#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <MeshUtils/PointLocator.h>

#include "Material.h"

class ElementWiseMaterial : public Material {
    public:
        typedef Mesh::Ptr MeshPtr;
        typedef Material::Ptr MaterialPtr;

        ElementWiseMaterial(Float density, MeshPtr material_mesh) :
            m_density(density),
            m_material_mesh(material_mesh),
            m_locator(material_mesh) {}
        virtual ~ElementWiseMaterial() {}

    public:
        virtual Float get_material_tensor(size_t i, size_t j, size_t k, size_t l, VectorF coord) const;
        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const;
        virtual Float get_density(VectorF coord) const { return m_density; }
        virtual Float get_density() const { return m_density; }

    protected:
        VectorI look_up_voxels(const VectorF& coords) const;

    protected:
        MeshPtr m_material_mesh;
        Float m_density;
        std::vector<MaterialPtr> m_materials;
        mutable PointLocator m_locator;
};
