/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "PatternParameter.h"

namespace PyMesh {

class VertexIsotropicOffsetParameter : public PatternParameter {
    public:
        /**
         * The argument dof_dir determines the offset direction.  The offset
         * magnitude is determines by cell size and parameter value.
         *
         *   seed_offset = (bbox_max - bbox_center) .* dof_dir * m_value
         *
         * where ".*" is component-wise multiplication.  Seed offset is then
         * symmetrically propagated to other verices in the same roi.
         */
        VertexIsotropicOffsetParameter(
                WireNetwork::Ptr wire_network, const VectorF& dof_dir);
        virtual ~VertexIsotropicOffsetParameter() {}

    public:
        virtual void apply(VectorF& results, const Variables& vars);
        virtual MatrixFr compute_derivative() const;
        virtual ParameterType get_type() const { return VERTEX_OFFSET; }

    protected:
        virtual void process_roi();

    private:
        VectorF m_dof_dir;
        std::vector<MatrixF> m_transforms;
};

}
