/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MiscChecker.h"

void MiscChecker::check_normals(FEMeshPtr mesh) {
    const size_t num_bd_faces = mesh->getNbrBoundaryFaces();
    const size_t num_bd_nodes = mesh->getNbrBoundaryNodes();
    for (size_t i=0; i<num_bd_faces; i++) {
        VectorF normal = mesh->getBoundaryFaceNormal(i);
        ASSERT_FLOAT_EQ(1.0, normal.norm());
    }

    for (size_t i=0; i<num_bd_nodes; i++) {
        VectorF normal = mesh->getBoundaryNodeNormal(i);
        ASSERT_FLOAT_EQ(1.0, normal.norm());
    }
}

void MiscChecker::check_volumes(FEMeshPtr mesh) {
    const size_t num_elements = mesh->getNbrElements();
    for (size_t i=0; i<num_elements; i++) {
        Float vol = mesh->getElementVolume(i);
        ASSERT_GT(vol, 0.0);
    }
}

void MiscChecker::check_areas(FEMeshPtr mesh) {
    const size_t num_bd_faces = mesh->getNbrBoundaryFaces();
    for (size_t i=0; i<num_bd_faces; i++) {
        Float area = mesh->getBoundaryFaceArea(i);
        ASSERT_GT(area, 0.0);
    }
}
