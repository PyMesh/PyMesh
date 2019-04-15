/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#include "GraphLaplacianAssembler.h"

#include <Core/EigenTypedef.h>

using namespace PyMesh;

ZSparseMatrix GraphLaplacianAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;
    typedef Eigen::Triplet<Float, size_t> T;
    std::vector<T> entries;

    auto mesh = setting->get_mesh();

    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_elements = mesh->getNbrElements();
    const size_t node_per_element = mesh->getNodePerElement();

    std::vector<std::list<size_t>> node_adjacency_list(num_nodes);
    for (size_t i=0; i<num_elements; i++) {
        const auto elem = mesh->getElement(i);
        for (size_t j=0; j<node_per_element; j++) {
            const size_t v0 = elem[j];
            for (size_t k=0; k<node_per_element; k++) {
                if (j == k) continue;
                const size_t v1 = elem[k];
                node_adjacency_list[v0].push_back(v1);
                node_adjacency_list[v1].push_back(v0);
            }
        }
    }

    for (size_t i=0; i<num_nodes; i++) {
        auto& adj_nodes = node_adjacency_list[i];
        adj_nodes.sort();
        adj_nodes.unique();

        const size_t num_adj_nodes = adj_nodes.size();
        entries.push_back({i, i, -1.0});
        for (size_t j : adj_nodes) {
            entries.push_back({i, j, 1.0 / num_adj_nodes});
        }
    }

    ZSparseMatrix L(num_nodes, num_nodes);
    L.setFromTriplets(entries.begin(), entries.end());
    return L;
}
