#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>

using ::testing::Contains;

namespace BoundaryChecker {
    typedef FEMeshAdaptor::Ptr FEMeshPtr;

    void check_boundary_node(FEMeshPtr mesh);
    void check_boundary_elements(FEMeshPtr mesh);
    void check_boundary_faces(FEMeshPtr mesh);
}
