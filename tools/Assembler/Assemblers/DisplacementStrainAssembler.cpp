/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "DisplacementStrainAssembler.h"

#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/ShapeFunctions/FEBasis.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/Math/MatrixOrder.h>

using namespace PyMesh;

ZSparseMatrix DisplacementStrainAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;
    typedef FESetting::FEBasisPtr FEBasisPtr;

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    FEMeshPtr mesh = setting->get_mesh();
    FEBasisPtr basis = setting->get_basis();

    const size_t dim = mesh->getDim();
    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t nodes_per_element = mesh->getNodePerElement();
    MatrixI order = MatrixOrder::get_order(dim);
    size_t num_entries_per_element = dim * (dim+1) / 2;

    for (size_t i=0; i<num_elements; i++) {
        VectorI elem = mesh->getElement(i);
        MatrixFr grads(nodes_per_element, dim);
        VectorF coord = VectorF::Ones(nodes_per_element) / nodes_per_element;
        for (size_t j=0; j<nodes_per_element; j++) {
            grads.row(j) = basis->evaluate_grad(i, j, coord);
        }

        size_t row_base = num_entries_per_element * i;
        for (size_t j=0; j<dim; j++) {
            for (size_t k=j; k<dim; k++) {
                for (size_t l=0; l<nodes_per_element; l++) {
                    entries.push_back(T(
                                row_base+order(j, k),
                                elem[l] * dim + k,
                                0.5 * grads(l, j) ));
                    entries.push_back(T(
                                row_base+order(k, j),
                                elem[l] * dim + j,
                                0.5 * grads(l, k) ));
                }
            }
        }
    }

    ZSparseMatrix B(num_elements * num_entries_per_element, num_nodes * dim);
    B.setFromTriplets(entries.begin(), entries.end());
    return B;
}
