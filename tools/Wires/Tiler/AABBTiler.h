/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "TilerEngine.h"

namespace PyMesh {

class AABBTiler : public TilerEngine {
    public:
        AABBTiler(WireNetwork::Ptr unit_wire_network,
                const VectorF& bbox_min,
                const VectorF& bbox_max,
                const VectorI& repetitions);
        virtual ~AABBTiler() {}

    public:
        virtual WireNetwork::Ptr tile();

    protected:
        void evaluate_parameters(
                WireNetwork& wire_network, const FuncList& funcs);
        void evaluate_thickness_parameters(
                WireNetwork& wire_network, const FuncList& funcs);
        void evaluate_offset_parameters(
                WireNetwork& wire_network, const FuncList& funcs);

    private:
        VectorF m_bbox_min;
        VectorF m_bbox_max;
        VectorI m_repetitions;
};

}
