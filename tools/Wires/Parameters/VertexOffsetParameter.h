#pragma once

#include "PatternParameter.h"

class VertexOffsetParameter : public PatternParameter {
    public:
        VertexOffsetParameter(WireNetwork::Ptr wire_network, size_t axis);
        virtual ~VertexOffsetParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return VERTEX_OFFSET; }

    private:
        size_t m_axis;
};
