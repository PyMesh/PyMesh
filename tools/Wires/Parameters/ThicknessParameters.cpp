/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ThicknessParameters.h"
#include "VertexThicknessParameter.h"
#include "EdgeThicknessParameter.h"

#include <Core/Exception.h>

using namespace PyMesh;

void ThicknessParameters::add(const VectorI& roi,
        const std::string& formula, Float value) {
    if (m_type == ParameterCommon::VERTEX) {
        m_params.emplace_back(
                PatternParameter::Ptr(new
                    VertexThicknessParameter(m_wire_network)));
    } else if (m_type == ParameterCommon::EDGE) {
        m_params.emplace_back(
                PatternParameter::Ptr(new
                    EdgeThicknessParameter(m_wire_network)));
    } else {
        throw NotImplementedError("Unknown thickness parameter type");
    }

    PatternParameter::Ptr param = m_params.back();
    param->set_roi(roi);
    param->set_value(value);
    param->set_formula(formula);
}

VectorF ThicknessParameters::evaluate(
        const ThicknessParameters::Variables& vars) {
    size_t size = 0;
    if (m_type == ParameterCommon::VERTEX) {
        size = m_wire_network->get_num_vertices();
    } else {
        size = m_wire_network->get_num_edges();
    }

    VectorF results = VectorF::Ones(size) * m_default_thickness;
    for (auto param : m_params) {
        param->apply(results, vars);
    }

    return results;
}

