#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "ElementWiseMaterial.h"

class ElementWiseIsotropicMaterial : public ElementWiseMaterial {
    public:
        ElementWiseIsotropicMaterial(Float density, MeshPtr material_mesh,
                const std::string& young_field_name,
                const std::string& poisson_field_name);

        virtual ~ElementWiseIsotropicMaterial() {};

    public:
        virtual Float get_material_tensor(size_t i, size_t j, size_t k, size_t l, VectorF coord) const;
        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const;

        virtual void update();

    private:
        void update_2D();
        void update_3D();

    private:
        typedef Material::Ptr MaterialPtr;
        std::vector<MaterialPtr> m_materials;
        const std::string m_young_field_name;
        const std::string m_poisson_field_name;
};
