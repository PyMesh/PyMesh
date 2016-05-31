/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "WireAttribute.h"

namespace PyMesh {

class WireVertexSymmetryAttribute : public WireAttribute {
    public:
        WireVertexSymmetryAttribute() {
            set_attribute_type(VERTEX);
        }
        virtual ~WireVertexSymmetryAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};

}
