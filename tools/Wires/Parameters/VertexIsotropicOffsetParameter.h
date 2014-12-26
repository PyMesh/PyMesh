#pragma once

#include "PatternParameter.h"

class VertexIsotropicOffsetParameter : public PatternParameter {
    public:
        VertexIsotropicOffsetParameter(WireNetwork::Ptr wire_network, size_t axis);
        virtual ~VertexIsotropicOffsetParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return VERTEX_OFFSET; }
        size_t get_axis() const { return m_axis; }

    protected:
        virtual void process_roi();

    private:
        size_t m_axis;
        std::vector<MatrixF> m_transforms;
};
