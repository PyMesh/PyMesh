/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "PatternParameter.h"

namespace PyMesh {

class VertexOffsetParameter : public PatternParameter {
    public:
        VertexOffsetParameter(WireNetwork::Ptr wire_network, size_t axis);
        virtual ~VertexOffsetParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return VERTEX_OFFSET; }
        size_t get_axis() const { return m_axis; }

    private:
        size_t m_axis;
};

}
