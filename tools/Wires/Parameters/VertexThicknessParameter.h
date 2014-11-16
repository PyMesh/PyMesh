#pragma once

#include "PatternParameter.h"

class VertexThicknessParameter : public PatternParameter {
    public:
        VertexThicknessParameter(WireNetwork::Ptr wire_network)
            : PatternParameter(wire_network) {}
        virtual ~VertexThicknessParameter() {}

    public:
        void apply(VectorF& results, const Variables& vars);
};
