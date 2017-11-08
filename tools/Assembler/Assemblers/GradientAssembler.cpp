/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "GradientAssembler.h"

#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/ShapeFunctions/FEBasis.h>
#include <Assembler/FESetting/FESetting.h>

using namespace PyMesh;

ZSparseMatrix GradientAssembler::assemble(FESettingPtr setting) {
    auto mesh = setting->get_mesh();
    auto basis = setting->get_basis();

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    const size_t dim = mesh->getDim();
    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t nodes_per_element = mesh->getNodePerElement();
    const VectorF coord = VectorF::Ones(nodes_per_element) / nodes_per_element;
    entries.reserve(num_elements * nodes_per_element * dim);
    for (size_t i=0; i<num_elements; i++) {
        VectorI elem = mesh->getElement(i);
        MatrixFr grads(nodes_per_element, dim);
        for (size_t j=0; j<nodes_per_element; j++) {
            grads.row(j) = basis->evaluate_grad(i, j, coord);
        }

        for (size_t j=0; j<nodes_per_element; j++) {
            for (size_t k=0; k<dim; k++) {
                entries.push_back(T(i*dim+k, elem[j], grads(j, k)));
            }
        }
    }

    ZSparseMatrix G(num_elements * dim, num_nodes);
    G.setFromTriplets(entries.begin(), entries.end());
    return G;
}
