/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "ElementWiseMaterial.h"

namespace PyMesh {

class ElementWiseOrthotropicMaterial : public ElementWiseMaterial {
    public:
        typedef std::vector<std::string> FieldNames;
        ElementWiseOrthotropicMaterial(Float density, MeshPtr material_mesh,
                const FieldNames& young_field_names,
                const FieldNames& poisson_field_names,
                const FieldNames& shear_field_names);

        virtual ~ElementWiseOrthotropicMaterial() {};

    public:
        virtual void update();

    private:
        void update_2D();
        void update_3D();

    private:
        const FieldNames m_young_field_names;
        const FieldNames m_poisson_field_names;
        const FieldNames m_shear_field_names;
};

}
