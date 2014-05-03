#pragma once

#include "UniformMaterial.h"

#include <sstream>

#include <Core/Exception.h>

/**
 * Symmetric material class assumes full symmetry in the material tensor.
 * Namely C_ijkl = C_jikl, C_ijkl = C_ijlk and C_ijkl = C_klij.
 * This allows us to reduce a 4th order material tensor into a symmetric matrix.
 */
class SymmetricMaterial : public UniformMaterial {
    public:
        /**
         * The material matrix should take engineer strain and map to
         * stress.  Mathematically:
         *
         *      [stress_00]       [strain_00            ]
         *      [stress_11]       [strain_11            ]
         *      [stress_22] = C * [strain_22            ]
         *      [stress_01]       [strain_01 + strain_10]
         *      [stress_02]       [strain_02 + strain_20]
         *      [stress_12]       [strain_12 + strain_21]
         *
         * This way allows the matrix C to be symmetric.  In contrast the matrix
         * that maps true strain to stress is not symmetric in general.
         */
        SymmetricMaterial(Float density, const MatrixF& material_matrix) {
            m_density = density;
            m_material_tensor = material_matrix;
            if (m_material_tensor.rows() == 6) {
                initialize_3D();
            } else if (m_material_tensor.rows() == 3) {
                initialize_2D();
            } else {
                std::stringstream err_msg;
                err_msg << "Material matrix size " << m_material_tensor.rows()
                    << " is not supported";
                throw NotImplementedError(err_msg.str());
            }
        }

        virtual Float get_material_tensor(size_t i, size_t j, size_t k, size_t l, VectorF coord) const {
            const size_t row = m_index_map(i,j);
            const size_t col = m_index_map(k,l);
            return m_material_tensor.coeff(row, col);
        }

        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const {
            if (m_dim == 2) {
                return strain_to_stress_2D(strain);
            } else if (m_dim == 3) {
                return strain_to_stress_3D(strain);
            } else {
                std::stringstream err_msg;
                err_msg << "Unsupport dimention: " << m_dim;
                throw NotImplementedError(err_msg.str());
            }
        }

    protected:
        SymmetricMaterial() {}

        void initialize_2D() {
            m_dim = 2;
            m_index_map.resize(2, 2);
            m_index_map << 0, 2,
                           2, 1;
        }

        void initialize_3D() {
            m_dim = 3;
            m_index_map.resize(3,3);
            m_index_map << 0, 3, 4,
                           3, 1, 5,
                           4, 5, 2;
        }

        MatrixF strain_to_stress_2D(const MatrixF& strain) const {
            assert(strain.rows() == 2);
            assert(strain.cols() == 2);
            assert(strain == strain.transpose());
            Vector3F v_strain(strain(0, 0), strain(1, 1), strain(0, 1) + strain(1, 0));
            Vector3F v_stress = m_material_tensor * v_strain;
            MatrixF stress(2, 2);
            stress << v_stress[0], v_stress[2],
                      v_stress[2], v_stress[1];
            return stress;
        }

        MatrixF strain_to_stress_3D(const MatrixF& strain) const {
            assert(strain.rows() == 3);
            assert(strain.cols() == 3);
            assert(strain == strain.transpose());
            VectorF v_strain(6);
            v_strain << strain(0, 0), strain(1, 1), strain(2, 2),
                     strain(0, 1) + strain(1, 0),
                     strain(0, 2) + strain(2, 0),
                     strain(1, 2) + strain(2, 1);
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
