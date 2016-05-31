/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "StiffnessAssembler.h"

#include <iostream>
#include <vector>

#include <Core/EigenTypedef.h>

//#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/ShapeFunctions/FEBasis.h>
#include <Assembler/Materials/Material.h>
#include <Assembler/FESetting/FESetting.h>

using namespace PyMesh;

ZSparseMatrix StiffnessAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;
    typedef FESetting::FEBasisPtr FEBasisPtr;
    typedef FESetting::MaterialPtr MaterialPtr;

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    FEMeshPtr mesh = setting->get_mesh();
    FEBasisPtr basis = setting->get_basis();
    MaterialPtr material = setting->get_material();

    const size_t dim = mesh->getDim();
    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t nodes_per_element = mesh->getNodePerElement();

    for (size_t i=0; i<num_elements; i++) {
        const VectorI elem = mesh->getElement(i);
        VectorF coord = mesh->getElementCenter(i);
        Float density = material->get_density(coord);

        for (size_t j=0; j<nodes_per_element; j++) {
            for (size_t k=0; k<nodes_per_element; k++) {
                MatrixF coeff = basis->integrate_material_contraction(
                            i, j, k, material);
                for (size_t l=0; l<dim; l++) {
                    for (size_t n=0; n<dim; n++) {
                        entries.push_back(
                                T(elem[j]*dim+l, elem[k]*dim+n, coeff(l, n) * density));
                    }
                }
            }
        }
    }

    ZSparseMatrix K(num_nodes * dim, num_nodes * dim);
    K.setFromTriplets(entries.begin(), entries.end());

    return K;
}
