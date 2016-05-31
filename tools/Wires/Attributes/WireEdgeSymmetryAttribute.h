/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "WireAttribute.h"

namespace PyMesh {

class WireEdgeSymmetryAttribute : public WireAttribute {
    public:
        WireEdgeSymmetryAttribute() {
            set_attribute_type(EDGE);
        }
        virtual ~WireEdgeSymmetryAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};

}
