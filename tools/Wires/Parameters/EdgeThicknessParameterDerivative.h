#pragma once

#include "ParameterDerivative.h"

class EdgeThicknessParameterDerivative : public ParameterDerivative {
    public:
        EdgeThicknessParameterDerivative(
                Mesh::Ptr mesh, PatternParameter::Ptr param)
            : ParameterDerivative(mesh, param) { }

        virtual ~EdgeThicknessParameterDerivative() {}

    public:
        virtual MatrixFr compute();
};
