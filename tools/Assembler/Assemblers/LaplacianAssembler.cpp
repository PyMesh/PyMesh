/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "LaplacianAssembler.h"

#include <iostream>
#include <vector>

#include <Core/EigenTypedef.h>

//#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/ShapeFunctions/FEBasis.h>
#include <Assembler/Materials/Material.h>
#include <Assembler/FESetting/FESetting.h>

using namespace PyMesh;

ZSparseMatrix LaplacianAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;
    typedef FESetting::FEBasisPtr FEBasisPtr;
    typedef FESetting::MaterialPtr MaterialPtr;

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    FEMeshPtr mesh = setting->get_mesh();
    FEBasisPtr basis = setting->get_basis();
    MaterialPtr material = setting->get_material();

    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t nodes_per_element = mesh->getNodePerElement();

    for (size_t i=0; i<num_elements; i++) {
        const VectorI elem = mesh->getElement(i);
        VectorF coord = mesh->getElementCenter(i);
        Float density = material->get_density(coord);

        for (size_t j=0; j<nodes_per_element; j++) {
            for (size_t k=0; k<nodes_per_element; k++) {
                Float grad_prod = basis->integrate_grad_grad(i, j, k);
                entries.push_back(T(elem[j], elem[k], grad_prod * density));
            }
        }
    }

    ZSparseMatrix L(num_nodes, num_nodes);
    L.setFromTriplets(entries.begin(), entries.end());
    return L;
}
