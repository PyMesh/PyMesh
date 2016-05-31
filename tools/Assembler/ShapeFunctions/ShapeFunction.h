/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>

#include <Core/EigenTypedef.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/Elements/Elements.h>

namespace PyMesh {

/**
 * Shape function represent the basis function of the finite element space for
 * an scalar field defined over the mesh.
 */
class ShapeFunction {
    public:
        typedef std::shared_ptr<ShapeFunction> Ptr;
        //typedef FEMeshAdaptor::Ptr FEMeshPtr;
        typedef Elements::Ptr FEMeshPtr;

        static Ptr create(FEMeshPtr mesh, const std::string& func_type);

    public:
        /**
         * Evalute shape function on element.
         * @param elem_idx       index of target element.
         * @param local_func_idx index of shape functions that is not zero over
         *                       element.
         * @param coord          barycentric coordinate of the target point
         *                       within element.
         */
        virtual Float evaluate_func(size_t elem_idx, size_t local_func_idx,
                const VectorF& coord)=0;

        /**
         * Evaluate shape function gradient on element.
         * @param elem_idx       index of target element.
         * @param local_func_idx index of shape functions that is not zero over
         *                       element.
         * @param coord          barycentric coordinate of the target point
         *                       within element.
         */
        virtual VectorF evaluate_grad(size_t elem_idx, size_t local_func_idx,
                const VectorF& coord)=0;
};

}
