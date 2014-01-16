#include "StiffnessAssembler.h"

#include <iostream>
#include <vector>

#include <Core/EigenTypedef.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/ShapeFunctions/Integrator.h>
#include <Assembler/Materials/Material.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/Math/ZSparseMatrix.h>

ZSparseMatrix StiffnessAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;
    typedef FESetting::IntegratorPtr IntegratorPtr;
    typedef FESetting::MaterialPtr MaterialPtr;

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    FEMeshPtr mesh = setting->get_mesh();
    IntegratorPtr integrator = setting->get_integrator();
    MaterialPtr material = setting->get_material();

    const size_t dim = mesh->getDim();
    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t nodes_per_element = mesh->getNodePerElement();

    for (size_t i=0; i<num_elements; i++) {
        const VectorI elem = mesh->getElement(i);

        for (size_t j=0; j<nodes_per_element; j++) {
            for (size_t k=0; k<nodes_per_element; k++) {
                MatrixF coeff = integrator->integrate_material_contraction(
                            i, j, k, material);
                for (size_t l=0; l<dim; l++) {
                    for (size_t n=0; n<dim; n++) {
                        entries.push_back(
                                T(elem[j]*dim+l, elem[k]*dim+n, coeff(l, n)));
                    }
                }
            }
        }
    }

    ZSparseMatrix K(num_nodes * dim, num_nodes * dim);
    K.setFromTriplets(entries.begin(), entries.end());
    return K;
}
