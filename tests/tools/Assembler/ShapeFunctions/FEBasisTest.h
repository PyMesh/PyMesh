/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Assembler/Elements/Elements.h>
#include <Assembler/ShapeFunctions/FEBasis.h>

#include <TestBase.h>

class FEBasisTest : public TestBase {
    protected:
        typedef Elements::Ptr FEMeshPtr;

        FEMeshPtr load_fem_mesh(const std::string& filename) {
            return Elements::adapt(load_mesh(filename));
        }
};

TEST_F(FEBasisTest, Creation) {
    FEMeshPtr cube_tet = load_fem_mesh("cube.msh");
    FEMeshPtr square = load_fem_mesh("square_2D.obj");

    FEBasis cube_tet_basis = FEBasis(cube_tet);
    FEBasis square_basis = FEBasis(square);
}
