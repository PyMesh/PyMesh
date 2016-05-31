/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>

#include <Core/EigenTypedef.h>
#include <Assembler/Elements/Elements.h>
#include <Assembler/Materials/Material.h>

#include "ShapeFunction.h"
#include "Integrator.h"

namespace PyMesh {

/**
 * This class is a facade of all concepts implemented here, including
 *   + Shape function
 *   + Integration of shape functions
 */
class FEBasis {
    public:
        typedef std::shared_ptr<FEBasis> Ptr;
        typedef Material::Ptr MaterialPtr;
        typedef ShapeFunction::Ptr ShapeFunctionPtr;
        typedef Integrator::Ptr IntegratorPtr;
        typedef Elements::Ptr FEMeshPtr;

        FEBasis(FEMeshPtr mesh);

    public:
        Float evaluate_func(size_t elem_idx,
                size_t local_func_idx, const VectorF& coord);

        VectorF evaluate_grad(size_t elem_idx,
                size_t local_func_idx, const VectorF& coord);

        Float integrate_func_func(size_t elem_idx,
                size_t local_func_i, size_t local_func_j);

        Float integrate_grad_grad(size_t elem_idx,
                size_t local_func_i, size_t local_func_j);

        Float integrate_grad_C_grad(size_t elem_idx,
                size_t local_func_i, size_t local_func_j, const MatrixF& C);

        MatrixF integrate_material_contraction(size_t elem_idx,
                size_t local_func_i, size_t local_func_j,
                const MaterialPtr material);

    private:
        FEMeshPtr m_mesh;
        ShapeFunctionPtr m_shape_func;
        IntegratorPtr m_integrator;
};

}
