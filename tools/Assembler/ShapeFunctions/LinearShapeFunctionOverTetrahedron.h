/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "ShapeFunction.h"

namespace PyMesh {

class LinearShapeFunctionOverTetrahedron : public ShapeFunction {
    public:
        LinearShapeFunctionOverTetrahedron(FEMeshPtr mesh);

    public:
        virtual Float evaluate_func(size_t elem_idx, size_t local_func_idx,
                const VectorF& coord);
        virtual VectorF evaluate_grad(size_t elem_idx, size_t local_func_idx,
                const VectorF& coord);

    private:
        void compute_shape_gradient();

    private:
        FEMeshPtr m_mesh;
        MatrixFr m_shape_grad;
};

}
