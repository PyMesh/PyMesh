/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <list>
#include <string>

#include <Wires/WireNetwork/WireNetwork.h>

#include "ParameterCommon.h"
#include "PatternParameter.h"
#include "ParameterList.h"

namespace PyMesh {

class ThicknessParameters : public ParameterList {
    public:
        ThicknessParameters(
                WireNetwork::Ptr wire_network,
                TargetType type,
                Float default_thickness) :
            m_wire_network(wire_network),
            m_type(type),
            m_default_thickness(default_thickness) {}

        virtual ~ThicknessParameters() {}

    public:
        void add(const VectorI& roi,
                const std::string& formula, Float value);

        VectorF evaluate(const Variables& vars);

        Float get_default() const { return m_default_thickness; }
        void set_default(Float value) { m_default_thickness = value; }

        TargetType get_type() const { return m_type; }

        WireNetwork::Ptr get_wire_network() const { return m_wire_network; }

    private:
        WireNetwork::Ptr m_wire_network;
        TargetType m_type;
        Float m_default_thickness;
};

}
