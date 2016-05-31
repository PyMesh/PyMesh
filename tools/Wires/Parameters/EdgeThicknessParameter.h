/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "PatternParameter.h"

namespace PyMesh {

class EdgeThicknessParameter : public PatternParameter {
    public:
        EdgeThicknessParameter(WireNetwork::Ptr wire_network)
            : PatternParameter(wire_network) {}
        virtual ~EdgeThicknessParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return EDGE_THICKNESS; }
};

}
