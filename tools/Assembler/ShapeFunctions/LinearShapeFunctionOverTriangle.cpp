/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "LinearShapeFunctionOverTriangle.h"

#include <cassert>
#include <sstream>

#include <Core/Exception.h>

#include "ShapeFunction.h"

using namespace PyMesh;

LinearShapeFunctionOverTriangle::LinearShapeFunctionOverTriangle(
        ShapeFunction::FEMeshPtr mesh) : m_mesh(mesh) {
    compute_shape_gradient();
}

Float LinearShapeFunctionOverTriangle::evaluate_func(
        size_t elem_idx, size_t local_func_idx, const VectorF& coord) {
    assert(local_func_idx < coord.size());
    return coord[local_func_idx];
}

VectorF LinearShapeFunctionOverTriangle::evaluate_grad(
        size_t elem_idx, size_t local_func_idx, const VectorF& coord) {
    assert(local_func_idx < coord.size());
    size_t idx = elem_idx * 3 + local_func_idx;
    return m_shape_grad.row(idx);
}

void LinearShapeFunctionOverTriangle::compute_shape_gradient() {
    const size_t dim = m_mesh->getDim();
    const size_t num_elements = m_mesh->getNbrElements();
    const size_t nodes_per_element = m_mesh->getNodePerElement();
    if (nodes_per_element != 3) {
        std::stringstream err_msg;
        err_msg << "Mesh incompatible with shape function" << std::endl;
        err_msg << "Expect nodes_per_element=3 instead we get " << nodes_per_element << std::endl;
        throw RuntimeError(err_msg.str());
    }
    if (dim != 2) {
        throw NotImplementedError(
                "Linear shape function for 3D triangles are not supported yet.");
    }

    MatrixF selector(nodes_per_element, dim);
    selector << 0.0, 0.0,
                1.0, 0.0,
                0.0, 1.0;

    m_shape_grad.resize(num_elements * nodes_per_element, dim);
    for (size_t i=0; i<num_elements; i++) {
        VectorI element = m_mesh->getElement(i);
        VectorF p[3] = {
            m_mesh->getNode(element[0]),
            m_mesh->getNode(element[1]),
            m_mesh->getNode(element[2])
        };

        MatrixF P(3,3);
        P <<     1.0,     1.0,     1.0,
             p[0][0], p[1][0], p[2][0],
             p[0][1], p[1][1], p[2][1];
        m_shape_grad.block<3,2>(i*3, 0) = P.inverse() * selector;
    }
}

