#pragma once

#include "PatternParameter.h"

class EdgeThicknessParameter : public PatternParameter {
    public:
        EdgeThicknessParameter(WireNetwork::Ptr wire_network)
            : PatternParameter(wire_network) {}
        virtual ~EdgeThicknessParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return EDGE_THICKNESS; }
};
