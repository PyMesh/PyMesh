#pragma once

#include <string>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Wires/WireNetwork/WireNetwork.h>

#include "ParameterCommon.h"

class PatternParameter {
    public:
        typedef std::shared_ptr<PatternParameter> Ptr;
        typedef ParameterCommon::Variables Variables;

    public:
        PatternParameter(WireNetwork::Ptr wire_network)
            : m_wire_network(wire_network) {}
        virtual ~PatternParameter() {}

        VectorI get_roi() const { return m_roi; }
        void set_roi(const VectorI& roi) { m_roi = roi; }

        Float get_value() const { return m_value; }
        void set_value(Float val) { m_value = val; }

        void set_formula(const std::string& formula) {
            m_formula = formula;
        }

    public:
        virtual void apply(VectorF& results, const Variables& vars)=0;

    protected:
        void evaluate_formula(const Variables& vars);

    protected:
        WireNetwork::Ptr m_wire_network;

        VectorI m_roi;
        Float m_value;
        std::string m_formula;
};
