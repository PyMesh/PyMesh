#pragma once

#include "WireAttribute.h"

class WireEdgeSymmetryAttribute : public WireAttribute {
    public:
        WireEdgeSymmetryAttribute() {
            set_attribute_type(EDGE);
        }
        virtual ~WireEdgeSymmetryAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};
