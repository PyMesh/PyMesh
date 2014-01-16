#pragma once
#include "UniformMaterial.h"

#include <iostream>
#include <Core/Exception.h>

template<size_t dim>
class IsotropicMaterial : public UniformMaterial {
    public:
        IsotropicMaterial(Float density, Float young_modulus, Float poisson_ratio);
};

template<>
class IsotropicMaterial<2> : public UniformMaterial {
    public:
        IsotropicMaterial(Float density, Float young_modulus, Float poisson_ratio) {
            m_dim = 2;
            m_density = density;
            m_material_tensor = MatrixF(3, 3);

            Float lambda = young_modulus * poisson_ratio /
                ((1.0 + poisson_ratio) * (1.0 - 2.0 * poisson_ratio));
            Float mu = young_modulus / (2.0 + 2.0 * poisson_ratio);
            m_material_tensor
                << lambda + 2*mu, lambda       , 0.0 ,
                   lambda       , lambda + 2*mu, 0.0 ,
                   0.0          , 0.0          , 2*mu;

            m_index_map.resize(2, 2);
            m_index_map << 0, 2,
                           2, 1;
        }

        virtual Float get_material_tensor(size_t i, size_t j, size_t k, size_t l) const {
            const size_t row = m_index_map(i, j);
            const size_t col = m_index_map(k, l);
            if (row == col && ((i != k) || (j != l))) {
                return 0;
            } else {
                return m_material_tensor.coeff(row, col);
            }
        }

        virtual MatrixF strain_to_stress(const MatrixF& strain) const {
            assert(strain.rows() == 2);
            assert(strain.cols() == 2);
            assert(strain == strain.transpose());
            Vector3F v_strain(strain(0, 0), strain(1, 1), strain(0, 1));
            Vector3F v_stress = m_material_tensor * v_strain;
            MatrixF stress(2, 2);
            stress << v_stress[0], v_stress[2],
                      v_stress[2], v_stress[1];
            return stress;
        }

    protected:
        MatrixF m_index_map;
};

template<>
class IsotropicMaterial<3> : public UniformMaterial {
    public:
        IsotropicMaterial(Float density, Float young_modulus, Float poisson_ratio) {
            m_dim = 3;
            m_density = density;
            m_material_tensor = MatrixF(6, 6);

            Float lambda = young_modulus * poisson_ratio /
                ((1.0 + poisson_ratio) * (1.0 - 2.0 * poisson_ratio));
            Float mu = young_modulus / (2.0 + 2.0 * poisson_ratio);
            m_material_tensor
                << lambda + 2*mu, lambda       , lambda       , 0.0 , 0.0 , 0.0 ,
                   lambda       , lambda + 2*mu, lambda       , 0.0 , 0.0 , 0.0 ,
                   lambda       , lambda       , lambda + 2*mu, 0.0 , 0.0 , 0.0 ,
                   0.0          , 0.0          , 0.0          , 2*mu, 0.0 , 0.0 ,
                   0.0          , 0.0          , 0.0          , 0.0 , 2*mu, 0.0 ,
                   0.0          , 0.0          , 0.0          , 0.0 , 0.0 , 2*mu;

            m_index_map.resize(3, 3);
            m_index_map << 0, 3, 4,
                           3, 1, 5,
                           4, 5, 2;
        }

        virtual Float get_material_tensor(size_t i, size_t j, size_t k, size_t l) const {
            size_t row = m_index_map(i, j);
            size_t col = m_index_map(k, l);
            if (row == col && ((i != k) || (j != l))) {
                return 0;
            } else {
                return m_material_tensor.coeff(row, col);
            }
        }

        virtual MatrixF strain_to_stress(const MatrixF& strain) const {
            assert(strain.rows() == 3);
            assert(strain.cols() == 3);
            assert(strain == strain.transpose());
            VectorF v_strain(6);
            v_strain << strain(0, 0), strain(1, 1), strain(2, 2), strain(0, 1), strain(0, 2) , strain(1, 2);
            VectorF v_stress = m_material_tensor * v_strain;
            MatrixF stress(3, 3);
            stress << v_stress[0], v_stress[3], v_stress[4],
                      v_stress[3], v_stress[1], v_stress[5],
                      v_stress[4], v_stress[5], v_stress[2];
            return stress;
        }

    protected:
        MatrixF m_index_map;
};

