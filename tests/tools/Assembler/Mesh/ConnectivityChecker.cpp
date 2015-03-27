/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ConnectivityChecker.h"

using namespace ConnectivityChecker;

void ConnectivityChecker::check_node_node_adjacency_symmetry(FEMeshPtr mesh) {
    const size_t num_nodes = mesh->getNbrNodes();
    for (size_t i=0; i<num_nodes; i++) {
        const VectorI adj_nodes_i = mesh->getNodeAdjacentNodes(i);
        for (size_t j=0; j<adj_nodes_i.size(); j++) {
            const VectorI adj_nodes_j = mesh->getNodeAdjacentNodes(adj_nodes_i[j]);
            std::vector<size_t> adj_nodes_vector_j(adj_nodes_j.data(),
                    adj_nodes_j.data() + adj_nodes_j.size());
            ASSERT_THAT(adj_nodes_vector_j, Contains(i));
        }
    }
}

void ConnectivityChecker::check_node_element_adjacency_symmetry(FEMeshPtr mesh) {
    const size_t num_nodes = mesh->getNbrNodes();
    for (size_t i=0; i<num_nodes; i++) {
        const VectorI adj_elem = mesh->getNodeAdjacentElements(i);
        for (size_t j=0; j<adj_elem.size(); j++) {
            const VectorI elem = mesh->getElement(adj_elem[j]);
            std::vector<size_t> elem_vector(elem.data(),
                    elem.data() + elem.size());
            ASSERT_THAT(elem_vector, Contains(i));
        }
    }
}

void ConnectivityChecker::check_element_element_adjacency_symmetry(FEMeshPtr mesh) {
    const size_t num_elements = mesh->getNbrElements();
    for (size_t i=0; i<num_elements; i++) {
        const VectorI adj_elem_i = mesh->getElementAdjacentElements(i);
        for (size_t j=0; j<adj_elem_i.size(); j++) {
            const VectorI adj_elem_j = mesh->getElementAdjacentElements(
                    adj_elem_i[j]);
            std::vector<size_t> adj_elem_vector_j(
                    adj_elem_j.data(), adj_elem_j.data() + adj_elem_j.size());
            ASSERT_THAT(adj_elem_vector_j, Contains(i));
        }
    }
}

void ConnectivityChecker::check_boundary_node_node_adjacency_symmetry(FEMeshPtr mesh) {
    const size_t num_bd_nodes = mesh->getNbrBoundaryNodes();
    for (size_t i=0; i<num_bd_nodes; i++) {
        VectorI adj_bd_nodes_i = mesh->getBoundaryNodeAdjacentBoundaryNodes(i);
        for (size_t j=0; j<adj_bd_nodes_i.size(); j++) {
            ASSERT_LT(adj_bd_nodes_i[j], num_bd_nodes);
            size_t bvj = adj_bd_nodes_i[j];
            VectorI adj_bd_nodes_j =
                mesh->getBoundaryNodeAdjacentBoundaryNodes(bvj);
            std::vector<size_t> adj_bd_nodes_vector_j(
                    adj_bd_nodes_j.data(),
                    adj_bd_nodes_j.data() + adj_bd_nodes_j.size());
            ASSERT_THAT(adj_bd_nodes_vector_j, Contains(i));
        }
    }
}

void ConnectivityChecker::check_boundary_node_face_adjacency_symmetry(FEMeshPtr mesh) {
    const size_t num_bd_nodes = mesh->getNbrBoundaryNodes();
    for (size_t i=0; i<num_bd_nodes; i++) {
        size_t vi = mesh->getBoundaryNode(i);
        VectorI adj_bd_faces = mesh->getBoundaryNodeAdjacentBoundaryFaces(i);
        for (size_t j=0; j<adj_bd_faces.size(); j++) {
            size_t bfj = adj_bd_faces[j];
            ASSERT_LT(bfj, mesh->getNbrBoundaryFaces());
            VectorI face = mesh->getBoundaryFace(bfj);
            std::vector<size_t> face_vector(
                    face.data(), face.data() + face.size());
            ASSERT_THAT(face_vector, Contains(vi));
        }
    }
}

