#pragma once

#include "WireAttribute.h"

class WireEdgeLengthAttribute : public WireAttribute {
    public:
        WireEdgeLengthAttribute() : WireAttribute() {
            set_attribute_type(EDGE);
        }
        virtual ~WireEdgeLengthAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};
