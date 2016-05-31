/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "Assembler.h"

namespace PyMesh {

/**
 * Assemble matrix that convert flattened strain to flattened stress.
 * Given the following strain matrix:
 *      [ e_xx e_xy e_xz ]
 *      [ e_yx e_yy e_yz ]
 *      [ e_zx e_zy e_zz ]
 *
 * Let C denote the assembled matrix, it satisfy the following equation:
 *
 * [sigma_xx]     [e_xx]
 * [sigma_yy]     [e_yy]
 * [sigma_zz] = C [e_zz]
 * [sigma_yz]     [e_yz]
 * [sigma_zx]     [e_zx]
 * [sigma_xy]     [e_xy]
 */
class ElasticityTensorAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};

}
