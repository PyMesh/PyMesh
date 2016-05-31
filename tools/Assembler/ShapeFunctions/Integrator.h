/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>

#include <Core/EigenTypedef.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/ShapeFunctions/ShapeFunction.h>
#include <Assembler/Materials/Material.h>

namespace PyMesh {

class Integrator {
    public:
        typedef std::shared_ptr<Integrator> Ptr;
        typedef Elements::Ptr FEMeshPtr;
        typedef ShapeFunction::Ptr ShapeFuncPtr;

        static Ptr create(FEMeshPtr mesh, ShapeFuncPtr);

    public:
        /**
         * Compute \int_{elem} \phi_i \phi_j \dd V
         */
        virtual Float integrate_func(size_t elem_idx,
                size_t local_func_i, size_t local_func_j)=0;

        /**
         * Compute \int_{elem} \nabla \phi_i^T \nabla \phi_j \dd V
         */
        virtual Float integrate_grad(size_t elem_idx,
                size_t local_func_i, size_t local_func_j)=0;

        /**
         * Compute \int_{elem} \nabla \phi_i^T C \nabla \phi_j \dd V
         */
        virtual Float integrate_grad_C(size_t elem_idx,
                size_t local_func_i, size_t local_func_j, const MatrixF& C)=0;

        /**
         * Compute the coefficient of u_{*}v_{*} in the following form:
         *
         *              [ u_{ix} \nabla \phi_i ]       [ v_{jx} \nabla \phi_j ]
         *  \int_{elem} [ u_{iy} \nabla \phi_i ] : C : [ v_{jy} \nabla \phi_j ] \dd V
         *              [ u_{iz} \nabla \phi_i ]       [ v_{jz} \nabla \phi_j ]
         *
         * where C is the material tensor.
         */
        virtual MatrixF integrate_material_contraction(size_t elem_idx,
                size_t local_func_i, size_t local_func_j,
                const Material::Ptr material)=0;
};

}
