/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Assembler/Elements/Elements.h>
#include <Mesh.h>

using ::testing::Contains;
using namespace PyMesh;

namespace GeometryChecker {
    typedef Elements::Ptr ElementsPtr;
    typedef Mesh::Ptr MeshPtr;

    void check_nodes(ElementsPtr m1, MeshPtr m2);
    void check_face_elements(ElementsPtr m1, MeshPtr m2);
    void check_edge_elements(ElementsPtr m1, MeshPtr m2);
    void check_voxel_elements(ElementsPtr m1, MeshPtr m2);
}
