#pragma once

#include "WireAttribute.h"

class WireVertexMinAngleAttribute : public WireAttribute {
    public:
        WireVertexMinAngleAttribute() {
            set_attribute_type(VERTEX);
        }
        virtual ~WireVertexMinAngleAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};
