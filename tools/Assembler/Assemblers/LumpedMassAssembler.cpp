/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "LumpedMassAssembler.h"

#include <vector>

#include <Core/EigenTypedef.h>

#include <Assembler/Materials/Material.h>
#include <Assembler/FESetting/FESetting.h>

using namespace PyMesh;

ZSparseMatrix LumpedMassAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;
    typedef FESetting::MaterialPtr MaterialPtr;

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    FEMeshPtr mesh = setting->get_mesh();
    MaterialPtr material = setting->get_material();

    const size_t dim = mesh->getDim();
    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t nodes_per_element = mesh->getNodePerElement();

    for (size_t i=0; i<num_elements; i++) {
        const VectorI elem = mesh->getElement(i);
        const Float ave_vol =
            mesh->getElementVolume(i) / nodes_per_element;
        VectorF coord = mesh->getElementCenter(i);
        Float density = material->get_density(coord);

        for (size_t j=0; j<nodes_per_element; j++) {
            entries.push_back(T(elem[j], elem[j], ave_vol * density));
        }
    }

    ZSparseMatrix M(num_nodes, num_nodes);
    M.setFromTriplets(entries.begin(), entries.end());
    M *= material->get_density();
    return M;
}
