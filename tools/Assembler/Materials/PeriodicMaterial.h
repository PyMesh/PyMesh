#pragma once
#include <vector>
#include "Material.h"

class PeriodicMaterial : public Material {
    public:
        PeriodicMaterial(Material::Ptr mat1, Material::Ptr mat2,
                VectorF axis, Float period);

        virtual Float get_material_tensor(
                size_t i, size_t j, size_t k, size_t l, VectorF coord) const;

        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const;

        virtual Float get_density(VectorF coord) const;

        virtual Float get_density() const;

    protected:
        PeriodicMaterial() {}

        size_t choose_material(VectorF coord) const;

    protected:
        Float m_period;
        VectorF m_axis;
        std::vector<Material::Ptr> m_materials;
};
