/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "PatternParameter.h"

namespace PyMesh {

class VertexCustomOffsetParameter : public PatternParameter {
    public:
        VertexCustomOffsetParameter(WireNetwork::Ptr wire_network,
                const MatrixFr& offset);
        virtual ~VertexCustomOffsetParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return VERTEX_OFFSET; }

    private:
        MatrixFr m_derivative;
};

}
