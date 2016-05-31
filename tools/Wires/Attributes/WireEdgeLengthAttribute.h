/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "WireAttribute.h"

namespace PyMesh {

class WireEdgeLengthAttribute : public WireAttribute {
    public:
        WireEdgeLengthAttribute() : WireAttribute() {
            set_attribute_type(EDGE);
        }
        virtual ~WireEdgeLengthAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};

}
