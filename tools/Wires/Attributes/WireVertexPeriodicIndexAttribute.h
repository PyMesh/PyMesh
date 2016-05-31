/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "WireAttribute.h"

namespace PyMesh {

/**
 * This attribute assign an index to each wire vertex.  Vertices share the same
 * index only if they are periodic vertices.  I.e. they coincide during tiling.
 * The indices may not form a continuous sequence.
 */
class WireVertexPeriodicIndexAttribute : public WireAttribute {
    public:
        WireVertexPeriodicIndexAttribute() {
            set_attribute_type(VERTEX);
        }

    public:
        virtual void compute(const WireNetwork& network);
};

}
