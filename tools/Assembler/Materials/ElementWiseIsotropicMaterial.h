/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "ElementWiseMaterial.h"

namespace PyMesh {

class ElementWiseIsotropicMaterial : public ElementWiseMaterial {
    public:
        ElementWiseIsotropicMaterial(Float density, MeshPtr material_mesh,
                const std::string& young_field_name,
                const std::string& poisson_field_name);

        virtual ~ElementWiseIsotropicMaterial() {};

    public:
        virtual void update();

    private:
        void update_2D();
        void update_3D();

    private:
        const std::string m_young_field_name;
        const std::string m_poisson_field_name;
};

}
