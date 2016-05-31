/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "OffsetParameters.h"
#include <iostream>
#include <cassert>

#include <Core/Exception.h>

#include "VertexOffsetParameter.h"
#include "VertexCustomOffsetParameter.h"
#include "VertexIsotropicOffsetParameter.h"

using namespace PyMesh;

OffsetParameters::OffsetParameters(WireNetwork::Ptr wire_network,
        OffsetParameters::TargetType type,
        Float default_value) :
    m_wire_network(wire_network),
    m_type(type),
    m_default_offset(default_value) {
        if (m_type == ParameterCommon::EDGE) {
            throw NotImplementedError("Only vertex offset is supported.");
        }
    }

void OffsetParameters::add(const VectorI& roi,
        const std::string& formula, Float value, size_t axis) {
    const Float tol = 1e-12;
    const size_t dim = m_wire_network->get_dim();
    const size_t num_vertices = m_wire_network->get_num_vertices();
    const VectorF bbox_min = m_wire_network->get_bbox_min();
    const VectorF bbox_max = m_wire_network->get_bbox_max();
    const VectorF bbox_center = 0.5 * (bbox_min + bbox_max);
    const MatrixFr& vertices = m_wire_network->get_vertices();
    assert(axis < dim);

    VectorF roi_min = bbox_max;
    VectorF roi_max = bbox_min;

    const size_t num_roi = roi.size();
    for (size_t i=0; i<num_roi; i++) {
        size_t v_idx = roi[i];
        assert(v_idx < num_vertices);
        const VectorF& v = vertices.row(v_idx);
        roi_min = roi_min.cwiseMin(v);
        roi_max = roi_max.cwiseMax(v);
    }

    if (fabs(roi_max[axis] - bbox_center[axis]) < tol &&
        fabs(roi_min[axis] - bbox_center[axis]) < tol) {
        // No dof in this axis without destroy symmetry.
        return;
    }
    if (roi_min[axis] > bbox_min[axis] + tol &&
            roi_max[axis] < bbox_max[axis] - tol) {
        m_params.emplace_back(PatternParameter::Ptr(
                    new VertexOffsetParameter(m_wire_network, axis)));
        PatternParameter::Ptr param = m_params.back();
        param->set_roi(roi);
        param->set_value(value);
        param->set_formula(formula);
    }
}

void OffsetParameters::add(const VectorI& roi,
        const std::string& formula, Float value,
        const MatrixFr& custom_offset) {
    m_params.emplace_back(PatternParameter::Ptr(
                new VertexCustomOffsetParameter(
                    m_wire_network, custom_offset)));
    PatternParameter::Ptr param = m_params.back();
    param->set_roi(roi);
    param->set_value(value);
    param->set_formula(formula);
}

void OffsetParameters::add_isotropic(const VectorI& roi,
        const std::string& formula, Float value, const VectorF& dof_dir) {
    m_params.emplace_back(PatternParameter::Ptr(
                new VertexIsotropicOffsetParameter(m_wire_network, dof_dir)));
    PatternParameter::Ptr param = m_params.back();
    param->set_roi(roi);
    param->set_value(value);
    param->set_formula(formula);
}

MatrixFr OffsetParameters::evaluate(const OffsetParameters::Variables& vars) {
    const size_t dim = m_wire_network->get_dim();
    const size_t size = m_wire_network->get_num_vertices();
    VectorF offset = VectorF::Ones(size * dim) * m_default_offset;
    for (auto param : m_params) {
        param->apply(offset, vars);
    }
    MatrixFr results = Eigen::Map<MatrixFr>(offset.data(), size, dim);
    return results;
}
