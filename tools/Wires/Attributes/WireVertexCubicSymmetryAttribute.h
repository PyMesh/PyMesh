#pragma once

#include "WireAttribute.h"

/**
 * Two vertices belong the same orbit if they can be mapped to each other by any
 * symmetry transformation that maps a cube to itself.
 */
class WireVertexCubicSymmetryAttribute : public WireAttribute {
    public:
        WireVertexCubicSymmetryAttribute() {
            set_attribute_type(VERTEX);
        }
        virtual ~WireVertexCubicSymmetryAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};
