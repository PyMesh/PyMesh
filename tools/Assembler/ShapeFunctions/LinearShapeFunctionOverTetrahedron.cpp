/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "LinearShapeFunctionOverTetrahedron.h"
#include <cassert>
#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

LinearShapeFunctionOverTetrahedron::LinearShapeFunctionOverTetrahedron(
        ShapeFunction::FEMeshPtr mesh) : m_mesh(mesh) {
    compute_shape_gradient();
}

Float LinearShapeFunctionOverTetrahedron::evaluate_func(
        size_t elem_idx, size_t local_func_idx, const VectorF& coord) {
    assert(local_func_idx < coord.size());
    return coord[local_func_idx];
}

VectorF LinearShapeFunctionOverTetrahedron::evaluate_grad(
        size_t elem_idx, size_t local_func_idx, const VectorF& coord) {
    assert(local_func_idx < coord.size());
    size_t idx = elem_idx * 4 + local_func_idx;
    return m_shape_grad.row(idx);
}

void LinearShapeFunctionOverTetrahedron::compute_shape_gradient() {
    const size_t dim = m_mesh->getDim();
    const size_t num_elements = m_mesh->getNbrElements();
    const size_t nodes_per_element = m_mesh->getNodePerElement();
    if (dim != 3 || nodes_per_element != 4) {
        std::stringstream err_msg;
        err_msg << "Mesh incompatible with shape function" << std::endl;
        err_msg << "Expect dim=3 instead we get " << dim << std::endl;
        err_msg << "Expect nodes_per_element=4 instead we get " << nodes_per_element << std::endl;
        throw RuntimeError(err_msg.str());
    }

    MatrixF selector(nodes_per_element, dim);
    selector << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0;

    m_shape_grad.resize(num_elements * nodes_per_element, dim);
    for (size_t i=0; i<num_elements; i++) {
        VectorI element = m_mesh->getElement(i);
        VectorF p[4] = {
            m_mesh->getNode(element[0]),
            m_mesh->getNode(element[1]),
            m_mesh->getNode(element[2]),
            m_mesh->getNode(element[3])
        };

        MatrixF P(4, 4);
        P <<     1.0,     1.0,     1.0,     1.0,
             p[0][0], p[1][0], p[2][0], p[3][0],
             p[0][1], p[1][1], p[2][1], p[3][1],
             p[0][2], p[1][2], p[2][2], p[3][2];

        m_shape_grad.block<4,3>(i*4, 0) = P.inverse() * selector;
    }
}

