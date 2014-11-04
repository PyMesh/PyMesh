#pragma once

#include "WireAttribute.h"

/**
 * This attribute assign an index to each wire edge.  Edges share the same
 * index only if they are periodic edges.  I.e. they coincide during tiling.
 * The indices may not form a continuous sequence.
 */
class WireEdgePeriodicIndexAttribute : public WireAttribute {
    public:
        WireEdgePeriodicIndexAttribute() {
            set_attribute_type(EDGE);
        }

    public:
        virtual void compute(const WireNetwork& network);
};
