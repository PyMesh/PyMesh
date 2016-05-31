/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "SymmetricMaterial.h"

#include <iostream>
#include <sstream>
#include <Core/Exception.h>

namespace PyMesh {

template<size_t dim>
class IsotropicMaterial : public SymmetricMaterial {
    public:
        IsotropicMaterial(Float density, Float young_modulus, Float poisson_ratio) {
            std::stringstream err_msg;
            err_msg << "Isotropic material in " << dim << "D is not supported.";
            throw NotImplementedError(err_msg.str());
        }
};

template<>
class IsotropicMaterial<2> : public SymmetricMaterial {
    public:
        IsotropicMaterial(Float density, Float young_modulus, Float poisson_ratio) {
            initialize(2);
            m_density = density;
            m_material_tensor = MatrixF(3, 3);

            // The stress to strain tensor, S, has the form:
            //     1   [ 1 -v   0 ]
            // S = - * [-v  1   0 ]
            //     E   [ 0  0 1+v ]
            //
            // Inverting it, C = S^-1, gives
            //       E     [ 1  v   0 ]
            // C = ----- * [ v  1   0 ]
            //     1-v^2   [ 0  0 1-v ]
            //
            // lambda and mu are derived from here.

            Float lambda = young_modulus * poisson_ratio /
                ((1.0 + poisson_ratio) * (1.0 - poisson_ratio));
            Float mu = young_modulus / (2.0 + 2.0 * poisson_ratio);

            // By the definition of symmetric material, the material tensor maps
            // from engineer strain to stress.  I.e. we need to scale C[2,2] by
            // 0.5
            m_material_tensor
                << lambda + 2*mu, lambda       , 0.0 ,
                   lambda       , lambda + 2*mu, 0.0 ,
                   0.0          , 0.0          , 1*mu;
        }
};

template<>
class IsotropicMaterial<3> : public SymmetricMaterial {
    public:
        IsotropicMaterial(Float density, Float young_modulus, Float poisson_ratio) {
            initialize(3);
            m_density = density;
            m_material_tensor = MatrixF(6, 6);

            // Lambda and mu are derived based on
            // https://www.efunda.com/formulae/solid_mechanics/mat_mechanics/hooke_isotropic.cfm
            Float lambda = young_modulus * poisson_ratio /
                ((1.0 + poisson_ratio) * (1.0 - 2.0 * poisson_ratio));
            Float mu = young_modulus / (2.0 + 2.0 * poisson_ratio);

            // By the definition of symmetric material, the material tensor maps
            // from engineer strain to stress.  I.e. we need to scale C[3,3],
            // C[4,4] and C[5,5] by 0.5
            m_material_tensor
                << lambda + 2*mu, lambda       , lambda       , 0.0 , 0.0 , 0.0 ,
                   lambda       , lambda + 2*mu, lambda       , 0.0 , 0.0 , 0.0 ,
                   lambda       , lambda       , lambda + 2*mu, 0.0 , 0.0 , 0.0 ,
                   0.0          , 0.0          , 0.0          , 1*mu, 0.0 , 0.0 ,
                   0.0          , 0.0          , 0.0          , 0.0 , 1*mu, 0.0 ,
                   0.0          , 0.0          , 0.0          , 0.0 , 0.0 , 1*mu;
        }
};

}
