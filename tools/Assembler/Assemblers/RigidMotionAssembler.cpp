#include "RigidMotionAssembler.h"

ZSparseMatrix RigidMotionAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    FEMeshPtr mesh = setting->get_mesh();

    const size_t dim = mesh->getDim();
    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t node_per_element = mesh->getNodePerElement();

    VectorF node_weight = VectorF::Zero(num_nodes);
    for (size_t i=0; i<num_elements; i++) {
        VectorI elem = mesh->getElement(i);
        Float volume = mesh->getElementVolume(i);
        for (size_t j=0; j<node_per_element; j++) {
            node_weight[elem[j]] += volume / node_per_element;
        }
    }
    node_weight /= node_weight.sum();

    // Translation part
    for (size_t i=0; i<num_nodes; i++) {
        for (size_t j=0; j<dim; j++) {
            entries.push_back(T(j,i*dim+j, node_weight[i]));
        }
    }

    // Rotation part
    size_t rot_degrees = 0;
    if (dim == 2) {
        rot_degrees = 1;
        for (size_t i=0; i<num_nodes; i++) {
            VectorF x = mesh->getNode(i) * node_weight[i];
            entries.push_back(T(dim  , i*dim  , -x[1]));
            entries.push_back(T(dim  , i*dim+1,  x[0]));
        }
    } else {
        rot_degrees = 3;
        for (size_t i=0; i<num_nodes; i++) {
            VectorF x = mesh->getNode(i) * node_weight[i];
            entries.push_back(T(dim  , i*dim+1, -x[2]));
            entries.push_back(T(dim  , i*dim+2,  x[1]));
            entries.push_back(T(dim+1, i*dim  ,  x[2]));
            entries.push_back(T(dim+1, i*dim+2, -x[0]));
            entries.push_back(T(dim+2, i*dim  , -x[1]));
            entries.push_back(T(dim+2, i*dim+1,  x[0]));
        }
    }

    ZSparseMatrix Ru(dim + rot_degrees, dim * num_nodes);
    Ru.setFromTriplets(entries.begin(), entries.end());
    return Ru;
}
