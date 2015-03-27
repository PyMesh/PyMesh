/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/Elements/TetrahedronElements.h>

#include <TestBase.h>

#include "GeometryChecker.h"

class TetrahedronElementsTest : public TestBase {
    protected:
        typedef Elements::Ptr ElementsPtr;

        ElementsPtr adapt(MeshPtr mesh) {
            return ElementsPtr(new TetrahedronElements(mesh));
        }
};

TEST_F(TetrahedronElementsTest, Dim) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    ElementsPtr cube_tet_elem = adapt(cube_tet);
    ASSERT_EQ(3, cube_tet_elem->getDim());
}

TEST_F(TetrahedronElementsTest, SurfaceMesh) {
    MeshPtr cube_tri = load_mesh("cube.obj");
    ASSERT_THROW(adapt(cube_tri), RuntimeError);
}

TEST_F(TetrahedronElementsTest, 2DMesh) {
    MeshPtr square = load_mesh("square_2D.obj");
    ASSERT_THROW(adapt(square), RuntimeError);
}

TEST_F(TetrahedronElementsTest, CheckNodes) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    ElementsPtr cube_tet_elem = adapt(cube_tet);

    using namespace GeometryChecker;
    check_nodes(cube_tet_elem, cube_tet);
}

TEST_F(TetrahedronElementsTest, CheckElements) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    ElementsPtr cube_tet_elem = adapt(cube_tet);

    using namespace GeometryChecker;
    check_voxel_elements(cube_tet_elem, cube_tet);
}

