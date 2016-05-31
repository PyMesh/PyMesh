/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>

using ::testing::Contains;
using namespace PyMesh;

namespace MiscChecker {
    typedef FEMeshAdaptor::Ptr FEMeshPtr;

    void check_normals(FEMeshPtr mesh);
    void check_volumes(FEMeshPtr mesh);
    void check_areas(FEMeshPtr mesh);
}
