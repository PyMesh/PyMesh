/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <list>
#include <string>

#include <Wires/WireNetwork/WireNetwork.h>

#include "ParameterCommon.h"
#include "PatternParameter.h"
#include "ParameterList.h"

namespace PyMesh {

class OffsetParameters : public ParameterList {
    public:
        OffsetParameters(WireNetwork::Ptr wire_network,
                TargetType type, Float default_value);

        virtual ~OffsetParameters() {}

    public:
        void add(const VectorI& roi,
                const std::string& formula, Float value, size_t axis);
        void add(const VectorI& roi,
                const std::string& formula, Float value,
                const MatrixFr& custom_offset);
        void add_isotropic(const VectorI& roi,
                const std::string& formula, Float value,
                const VectorF& dof_dir);

        MatrixFr evaluate(const Variables& vars);

        Float get_default() const { return m_default_offset; }
        void set_default(Float value) { m_default_offset = value; }

        TargetType get_type() const { return m_type; }

        WireNetwork::Ptr get_wire_network() const { return m_wire_network; }

    private:
        WireNetwork::Ptr m_wire_network;
        TargetType m_type;
        Float m_default_offset;
};

}
