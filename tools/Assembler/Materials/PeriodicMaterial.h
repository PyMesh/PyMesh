#pragma once
#include <vector>
#include "Material.h"

class PeriodicMaterial : public Material {
    public:
        /**
         * Create periodic material
         * @param axis    axis of periodicity (i.e. orthogonal to laminations).
         * @param period
         * @param ratio   material ratio of mat1 (between 0 and 1).
         * @param phase   starting phase shift of the period (between 0 and 1).
         */
        PeriodicMaterial(Material::Ptr mat1, Material::Ptr mat2,
                VectorF axis, Float period, Float ratio, Float phase);

        virtual Float get_material_tensor(
                size_t i, size_t j, size_t k, size_t l, VectorF coord) const;

        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const;

        virtual Float get_density(VectorF coord) const;

        virtual Float get_density() const;

    protected:
        PeriodicMaterial() {}

        size_t choose_material(VectorF coord) const;

        void check_validity() const;

    protected:
        Float m_period;
        Float m_ratio;
        Float m_phase;
        VectorF m_axis;
        std::vector<Material::Ptr> m_materials;
};
