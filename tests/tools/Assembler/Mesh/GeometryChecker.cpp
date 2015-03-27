/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "GeometryChecker.h"

using namespace GeometryChecker;

void GeometryChecker::check_nodes(FEMeshPtr m1, MeshPtr m2) {
    ASSERT_EQ(m1->getNbrNodes(), m2->get_num_vertices());
    ASSERT_EQ(m1->getNodes(), m2->get_vertices());
    const size_t num_nodes = m1->getNbrNodes();
    for (size_t i=0; i<num_nodes; i++) {
        ASSERT_EQ(m1->getNode(i), m2->get_vertex(i));
    }
}

void GeometryChecker::check_face_elements(FEMeshPtr m1,
        MeshPtr m2) {
    ASSERT_EQ(m1->getNbrElements(), m2->get_num_faces());
    ASSERT_EQ(m1->getElements(), m2->get_faces());
    const size_t num_elements = m1->getNbrElements();
    for (size_t i=0; i<num_elements; i++) {
        ASSERT_EQ(m1->getElement(i), m2->get_face(i));
    }
}

void GeometryChecker::check_voxel_elements(FEMeshPtr m1,
        MeshPtr m2) {
    ASSERT_EQ(m1->getNbrElements(), m2->get_num_voxels());
    ASSERT_EQ(m1->getElements(), m2->get_voxels());
    const size_t num_elements = m1->getNbrElements();
    for (size_t i=0; i<num_elements; i++) {
        ASSERT_EQ(m1->getElement(i), m2->get_voxel(i));
    }
}
