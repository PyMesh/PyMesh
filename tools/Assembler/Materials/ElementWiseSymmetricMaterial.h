#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "ElementWiseMaterial.h"

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
        typedef Material::Ptr MaterialPtr;
        std::vector<MaterialPtr> m_materials;
        const std::string m_matrix_field_name;
};
