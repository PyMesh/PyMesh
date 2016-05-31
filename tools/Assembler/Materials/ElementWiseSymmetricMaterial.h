/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "ElementWiseMaterial.h"

namespace PyMesh {

class ElementWiseSymmetricMaterial : public ElementWiseMaterial {
    public:
        ElementWiseSymmetricMaterial(Float density, MeshPtr material_mesh,
                const std::string& matrix_field_name);

        virtual ~ElementWiseSymmetricMaterial() {}

    public:
        virtual void update();

    private:
        void update_2D();
        void update_3D();

    private:
        const std::string m_matrix_field_name;
};

}
