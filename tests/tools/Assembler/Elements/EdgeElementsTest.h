/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/Elements/EdgeElements.h>

#include <TestBase.h>

#include "GeometryChecker.h"

class EdgeElementsTest : public TestBase {
    protected:
        typedef Elements::Ptr ElementsPtr;

        ElementsPtr adapt(MeshPtr mesh) {
            return ElementsPtr(new EdgeElements(mesh));
        }
};

TEST_F(EdgeElementsTest, 2DMesh) {
    MeshPtr square = load_mesh("square_2D.obj");
    ElementsPtr square_elem = adapt(square);
    ASSERT_EQ(2, square_elem->getDim());
}

TEST_F(EdgeElementsTest, SurfaceMesh) {
    MeshPtr cube_tri = load_mesh("cube.obj");
    ElementsPtr cube_tri_elem = adapt(cube_tri);
    ASSERT_EQ(3, cube_tri_elem->getDim());
}

TEST_F(EdgeElementsTest, VolumeMesh) {
    MeshPtr cube_tet = load_mesh("cube.msh");
    ElementsPtr cube_tet_elem = adapt(cube_tet);
    ASSERT_EQ(3, cube_tet_elem->getDim());
}

TEST_F(EdgeElementsTest, CheckNodes) {
    MeshPtr square = load_mesh("square_2D.obj");
    ElementsPtr square_elem = adapt(square);

    MeshPtr cube_tri = load_mesh("cube.obj");
    ElementsPtr cube_tri_elem = adapt(cube_tri);

    MeshPtr cube_tet = load_mesh("cube.msh");
    ElementsPtr cube_tet_elem = adapt(cube_tet);

    using namespace GeometryChecker;
    check_nodes(square_elem, square);
    check_nodes(cube_tri_elem, cube_tri);
    check_nodes(cube_tet_elem, cube_tet);
}

TEST_F(EdgeElementsTest, CheckElements) {
    MeshPtr square = load_mesh("square_2D.obj");
    ElementsPtr square_elem = adapt(square);

    MeshPtr cube_tri = load_mesh("cube.obj");
    ElementsPtr cube_tri_elem = adapt(cube_tri);

    MeshPtr cube_tet = load_mesh("cube.msh");
    ElementsPtr cube_tet_elem = adapt(cube_tet);

    ASSERT_EQ(4, square_elem->getNbrElements());
    ASSERT_EQ(0, cube_tri_elem->getNbrElements());
    ASSERT_EQ(0, cube_tet_elem->getNbrElements());
}

TEST_F(EdgeElementsTest, CheckElementsEntries) {
    MeshPtr square = load_mesh("square_2D.obj");
    ElementsPtr square_elem = adapt(square);

    using namespace GeometryChecker;
    check_edge_elements(square_elem, square);
}

