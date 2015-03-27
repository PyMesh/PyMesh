/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "WireAttribute.h"

class WireEdgeCubicSymmetryAttribute : public WireAttribute {
    public:
        WireEdgeCubicSymmetryAttribute() {
            set_attribute_type(EDGE);
        }
        virtual ~WireEdgeCubicSymmetryAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);
};
