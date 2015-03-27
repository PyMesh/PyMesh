/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "BoundaryChecker.h"

using namespace BoundaryChecker;

void BoundaryChecker::check_boundary_node(FEMeshPtr mesh) {
    const size_t num_nodes = mesh->getNbrNodes();
    const size_t num_boundary_nodes = mesh->getNbrBoundaryNodes();
    VectorI boundary_nodes = mesh->getBoundaryNodes();
    ASSERT_EQ(num_boundary_nodes, boundary_nodes.size());

    for (size_t i=0; i<num_boundary_nodes; i++) {
        size_t vi = boundary_nodes[i];
        ASSERT_LT(vi, num_nodes);
        ASSERT_TRUE(mesh->isBoundaryNode(vi));
        ASSERT_EQ(i, mesh->getBoundaryIndex(vi));
        ASSERT_EQ(vi, mesh->getBoundaryNode(i));
    }
}

void BoundaryChecker::check_boundary_elements(FEMeshPtr mesh) {
    const size_t num_boundary_nodes = mesh->getNbrBoundaryNodes();
    for (size_t i=0; i<num_boundary_nodes; i++) {
        size_t vi = mesh->getBoundaryNode(i);
        VectorI adj_elements = mesh->getNodeAdjacentElements(vi);
        for (size_t j=0; j<adj_elements.size(); j++) {
            ASSERT_TRUE(mesh->isBoundaryElement(adj_elements[j]));
        }
    }
}

void BoundaryChecker::check_boundary_faces(FEMeshPtr mesh) {
    const size_t num_boundary_faces = mesh->getNbrBoundaryFaces();
    for (size_t i=0; i<num_boundary_faces; i++) {
        VectorI face = mesh->getBoundaryFace(i);
        for (size_t j=0; j<face.size(); j++) {
            ASSERT_TRUE(mesh->isBoundaryNode(face[j]));
        }
    }
}
