#pragma once
#include "SymmetricMaterial.h"

#include <cassert>
#include <sstream>

#include <Core/Exception.h>

namespace OrthotropicMaterialHelper {
    void check_material_parameters(size_t dim, const VectorF& young_modulus, 
            const VectorF& poisson_ratio, const VectorF& shear_modulus) {
        if (dim != 2 && dim != 3) {
            throw NotImplementedError("Only 2D and 3D materials are supported");
        }
        if (dim != young_modulus.size()) {
            throw RuntimeError("Wrong dimention for young's modulus");
        }
        if (dim*(dim-1)/2 != shear_modulus.size()) {
            throw RuntimeError("Wrong dimention for shear modulus");
        }
        if (young_modulus.minCoeff() < 0.0) {
            throw RuntimeError("Young's modulus must be positive!");
        }
        if (shear_modulus.minCoeff() < 0.0) {
            throw RuntimeError("Shear modulus must be positive!");
        }
    }
}

using namespace OrthotropicMaterialHelper;

template<size_t dim>
class OrthotropicMaterial : public SymmetricMaterial {
    public:
        OrthotropicMaterial(Float density, const VectorF& young_modulus,
                const VectorF& poisson_ratio, const VectorF& shear_modulus) {
            std::stringstream err_msg;
            err_msg << "Orthotropic material in " << dim << "D is not supported.";
            throw NotImplementedError(err_msg.str());
        }
};

template<>
class OrthotropicMaterial<2> : public SymmetricMaterial {
    public:
        /**
         * @param density
         * @param young_modulus [young_x, young_y]
         * @param poisson_ratio [poisson_xy, poisson_yx]
         * @param shear_modulus [shear_xy]
         */
        OrthotropicMaterial(Float density, const VectorF& young_modulus,
                const VectorF& poisson_ratio, const VectorF& shear_modulus) {
            initialize_2D();
            m_density = density;
            check_material_parameters(m_dim, young_modulus, poisson_ratio, shear_modulus);

            Float young_x = young_modulus[0];
            Float young_y = young_modulus[1];

            Float shear_xy = shear_modulus[0];

            Float poisson_xy = poisson_ratio[0];
            Float poisson_yx = poisson_ratio[1];

            size_t tensor_size = m_dim * (m_dim + 1) / 2;
            MatrixF S = MatrixF::Zero(tensor_size, tensor_size);

            S << 1.0 / young_x, -poisson_xy / young_y, 0.0,
              -poisson_yx / young_x, 1.0 / young_y, 0.0,
              0.0, 0.0, 1.0 / shear_xy;
            m_material_tensor = S.inverse();
        }
};

template<>
class OrthotropicMaterial<3> : public SymmetricMaterial {
    public:
        /**
         * @param density 
         * @param young_modulus  [young_x, young_y, young_z]
         * @param poisson_ratio  [poisson_yz, poisson_zy,
         *                        poisson_zx, poisson_xz,
         *                        poisson_xy, poisson_yx]
         * @param shear_modulus  [shear_yz, shear_zx, shear_xy]
         */
        OrthotropicMaterial(Float density, const VectorF& young_modulus,
                const VectorF& poisson_ratio, const VectorF& shear_modulus) {
            initialize_3D();
            m_density = density;
            check_material_parameters(m_dim, young_modulus, poisson_ratio, shear_modulus);

            Float young_x = young_modulus[0];
            Float young_y = young_modulus[1];
            Float young_z = young_modulus[2];

            Float shear_yz = shear_modulus[0];
            Float shear_zx = shear_modulus[1];
            Float shear_xy = shear_modulus[2];

            Float poisson_yz = poisson_ratio[0];
            Float poisson_zy = poisson_ratio[1];
            Float poisson_zx = poisson_ratio[2];
            Float poisson_xz = poisson_ratio[3];
            Float poisson_xy = poisson_ratio[4];
            Float poisson_yx = poisson_ratio[5];

            size_t tensor_size = m_dim * (m_dim + 1) / 2;
            MatrixF S = MatrixF::Zero(tensor_size, tensor_size);

            S << 1.0 / young_x,
              -poisson_yx / young_y,
              -poisson_zx / young_z,
              0.0, 0.0, 0.0,

              -poisson_xy / young_x,
              1.0 / young_y,
              -poisson_zy / young_z,
              0.0, 0.0, 0.0,

              -poisson_xz / young_x,
              -poisson_yz / young_y,
              1.0 / young_z,
              0.0, 0.0, 0.0,

              0.0, 0.0, 0.0, 1.0 / shear_xy, 0.0, 0.0,
              0.0, 0.0, 0.0, 0.0, 1.0 / shear_zx, 0.0,
              0.0, 0.0, 0.0, 0.0, 0.0, 1.0 / shear_yz;

            m_material_tensor = S.inverse();
        }

};
