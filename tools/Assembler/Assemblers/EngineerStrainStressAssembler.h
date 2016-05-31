/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "Assembler.h"

namespace PyMesh {

/**
 * Assemble matrix that convert flattened engineer strain to flattened stress.
 * Imagine the following strain matrix:
 *      [ e_xx e_xy e_xz ]
 *      [ e_yx e_yy e_yz ]
 *      [ e_zx e_zy e_zz ]
 * Engineer strain is defined by replacing the shear components with engineering
 * shear strain:
 *      [ e_xx r_xy r_xz ]
 *      [ r_yx e_yy r_yz ]
 *      [ r_zx r_zy e_zz ]
 * where
 *      r_xy = e_xy + e_yx
 *      r_xz = e_xz + e_zx
 *      r_yx = e_yz + e_zy
 *
 * Let D denote the assembled matrix, it satisfy the following equation:
 *
 * [sigma_xx]     [e_xx]
 * [sigma_yy]     [e_yy]
 * [sigma_zz] = D [e_zz]
 * [sigma_yz]     [r_yz]
 * [sigma_zx]     [r_zx]
 * [sigma_xy]     [r_xy]
 *
 * See http://www.efunda.com/formulae/solid_mechanics/mat_mechanics/strain.cfm
 * for more details.
 */
class EngineerStrainStressAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};

}
