/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <MeshUtils/PointLocator.h>
#include <Misc/HashGrid.h>

#include "Material.h"

namespace PyMesh {

class ElementWiseMaterial : public Material {
    public:
        typedef Mesh::Ptr MeshPtr;
        typedef Material::Ptr MaterialPtr;

        ElementWiseMaterial(Float density, MeshPtr material_mesh);
        virtual ~ElementWiseMaterial() {}

    public:
        virtual Float get_material_tensor(size_t i, size_t j, size_t k, size_t l, VectorF coord) const;
        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const;
        virtual Float get_density(VectorF coord) const { return m_density; }
        virtual Float get_density() const { return m_density; }
        virtual size_t get_dim() const { return m_material_mesh->get_dim(); }

    protected:
        Float compute_cell_size();
        void initialize_2D_grid();
        void initialize_3D_grid();
        VectorI look_up_voxels(const VectorF& coords) const;

    protected:
        MeshPtr m_material_mesh;
        Float m_density;
        std::vector<MaterialPtr> m_materials;
        //mutable PointLocator m_locator;
        HashGrid::Ptr m_grid;
};

}
