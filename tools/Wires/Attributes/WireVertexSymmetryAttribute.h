#pragma once

#include "WireAttribute.h"

class WireVertexSymmetryAttribute : public WireAttribute {
    public:
        WireVertexSymmetryAttribute() {
            set_attribute_type(VERTEX);
        }
        virtual ~WireVertexSymmetryAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};
