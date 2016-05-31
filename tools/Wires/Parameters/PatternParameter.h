/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Wires/WireNetwork/WireNetwork.h>

#include "ParameterCommon.h"

namespace PyMesh {

class PatternParameter {
    public:
        typedef std::shared_ptr<PatternParameter> Ptr;
        typedef ParameterCommon::Variables Variables;
        enum ParameterType {
            VERTEX_THICKNESS,
            EDGE_THICKNESS,
            VERTEX_OFFSET
        };

    public:
        PatternParameter(WireNetwork::Ptr wire_network)
            : m_wire_network(wire_network) {}
        virtual ~PatternParameter() {}

        VectorI get_roi() const { return m_roi; }
        void set_roi(const VectorI& roi) {
            m_roi = roi;
            process_roi();
        }

        Float get_value() const { return m_value; }
        void set_value(Float val) { m_value = val; }

        void set_formula(const std::string& formula) {
            m_formula = formula;
        }

        const std::string& get_formula() const {
            return m_formula;
        }

        WireNetwork::Ptr get_wire_network() { return m_wire_network; }
        void set_wire_network(WireNetwork::Ptr network) { m_wire_network = network; }

    public:
        virtual void apply(VectorF& results, const Variables& vars)=0;

        /**
         * Compute dv/dp, i.e. gradient of wire vertex location with respect to
         * change in parameter.
         * Returned matrix is of size (num_vertex, dim).
         */
        virtual MatrixFr compute_derivative() const = 0;

        virtual ParameterType get_type() const =0;

    protected:
        virtual void process_roi() {};

        void evaluate_formula(const Variables& vars);

    protected:
        WireNetwork::Ptr m_wire_network;

        VectorI m_roi;
        Float m_value;
        std::string m_formula;
};

}
