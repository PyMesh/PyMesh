/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "ParameterDerivative.h"

namespace PyMesh {

class EdgeThicknessParameterDerivative : public ParameterDerivative {
    public:
        EdgeThicknessParameterDerivative(
                Mesh::Ptr mesh, PatternParameter::Ptr param)
            : ParameterDerivative(mesh, param) { }

        virtual ~EdgeThicknessParameterDerivative() {}

    public:
        virtual MatrixFr compute();
};

}
