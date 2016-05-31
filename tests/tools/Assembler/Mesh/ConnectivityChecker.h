/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>

using ::testing::Contains;
using namespace PyMesh;

namespace ConnectivityChecker {
    typedef FEMeshAdaptor::Ptr FEMeshPtr;

    void check_node_node_adjacency_symmetry(FEMeshPtr mesh);
    void check_node_element_adjacency_symmetry(FEMeshPtr mesh);
    void check_element_element_adjacency_symmetry(FEMeshPtr mesh);
    void check_boundary_node_node_adjacency_symmetry(FEMeshPtr mesh);
    void check_boundary_node_face_adjacency_symmetry(FEMeshPtr mesh);
}
