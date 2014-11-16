#pragma once

#include "PatternParameter.h"

class VertexOffsetParameter : public PatternParameter {
    public:
        VertexOffsetParameter(WireNetwork::Ptr wire_network, size_t axis);
        virtual ~VertexOffsetParameter() {}

    public:
        void apply(VectorF& results, const Variables& vars);

    private:
        size_t m_axis;
        VectorF m_center;
};
