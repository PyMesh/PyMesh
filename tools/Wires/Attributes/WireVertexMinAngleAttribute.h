/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "WireAttribute.h"

namespace PyMesh {

class WireVertexMinAngleAttribute : public WireAttribute {
    public:
        WireVertexMinAngleAttribute() {
            set_attribute_type(VERTEX);
        }
        virtual ~WireVertexMinAngleAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};

}
