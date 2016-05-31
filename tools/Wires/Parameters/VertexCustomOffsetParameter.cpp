/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexCustomOffsetParameter.h"

using namespace PyMesh;

VertexCustomOffsetParameter::VertexCustomOffsetParameter(
        WireNetwork::Ptr wire_network, const MatrixFr& offset) :
    PatternParameter(wire_network), m_derivative(offset) {
    }

void VertexCustomOffsetParameter::apply(
        VectorF& results, const Variables& vars) {
    const size_t dim = m_wire_network->get_dim();
    const size_t num_vertices = m_wire_network->get_num_vertices();
    const size_t roi_size = m_roi.size();
    assert(results.size() == dim * num_vertices);

    if (m_formula != "") evaluate_formula(vars);

    for (size_t i=0; i<roi_size; i++) {
        size_t v_idx = m_roi[i];
        assert(v_idx < num_vertices);
        results.segment(v_idx*dim, dim) += m_derivative.row(v_idx) * m_value;
    }
}

MatrixFr VertexCustomOffsetParameter::compute_derivative() const {
    const size_t num_vertices = m_wire_network->get_num_vertices();
    const size_t dim = m_wire_network->get_dim();
    const size_t roi_size = m_roi.size();

    MatrixFr derivative = MatrixFr::Zero(num_vertices, dim);
    for (size_t i=0; i<roi_size; i++) {
        size_t v_idx = m_roi[i];
        derivative.row(v_idx) += m_derivative.row(v_idx);
    }
    return derivative;
}
