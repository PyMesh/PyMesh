#pragma once

#include "PatternParameter.h"

class EdgeThicknessParameter : public PatternParameter {
    public:
        EdgeThicknessParameter(WireNetwork::Ptr wire_network)
            : PatternParameter(wire_network) {}
        virtual ~EdgeThicknessParameter() {}

    public:
        void apply(VectorF& results, const Variables& vars);
};
