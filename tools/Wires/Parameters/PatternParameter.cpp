/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PatternParameter.h"
#include <sstream>
#include <Core/Exception.h>

using namespace PyMesh;

void PatternParameter::evaluate_formula(
        const PatternParameter::Variables& vars) {
    Variables::const_iterator itr = vars.find(m_formula);
    if (itr == vars.end()) {
        std::stringstream err_msg;
        err_msg << "Cannot apply formula: " << m_formula;
        throw RuntimeError(err_msg.str());
    }
    m_value = itr->second;
}
