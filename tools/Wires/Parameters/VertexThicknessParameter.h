#pragma once

#include "PatternParameter.h"

class VertexThicknessParameter : public PatternParameter {
    public:
        VertexThicknessParameter(WireNetwork::Ptr wire_network)
            : PatternParameter(wire_network) {}
        virtual ~VertexThicknessParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return VERTEX_THICKNESS; }
};
