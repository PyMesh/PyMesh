/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Elements/Elements.h>
#include "GeometryChecker.h"

#include <TestBase.h>

class ElementsTest : public TestBase {
    protected:
        typedef Elements::Ptr ElementsPtr;

        ElementsPtr adapt(MeshPtr mesh) {
            return Elements::adapt(mesh);
        }

        ElementsPtr adapt_bd(MeshPtr mesh) {
            return Elements::adapt_boundary(mesh);
        }
};

TEST_F(ElementsTest, VolumeMesh) {
    MeshPtr mesh = load_mesh("cube.msh");
    ElementsPtr volume_elements = adapt(mesh);
    ElementsPtr boundary_elements = adapt_bd(mesh);
    ASSERT_EQ(6, volume_elements->getNbrElements());
    ASSERT_EQ(12, boundary_elements->getNbrElements());

    using namespace GeometryChecker;
    check_nodes(volume_elements, mesh);
    check_voxel_elements(volume_elements, mesh);
    check_nodes(boundary_elements, mesh);
    check_face_elements(boundary_elements, mesh);
}

TEST_F(ElementsTest, 2DMesh) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    ElementsPtr volume_elements = adapt(mesh);
    ElementsPtr boundary_elements = adapt_bd(mesh);
    ASSERT_EQ(2, volume_elements->getNbrElements());
    ASSERT_EQ(4, boundary_elements->getNbrElements());

    using namespace GeometryChecker;
    check_nodes(volume_elements, mesh);
    check_face_elements(volume_elements, mesh);
    check_nodes(boundary_elements, mesh);
    check_edge_elements(boundary_elements, mesh);
}

TEST_F(ElementsTest, SurfaceMesh) {
    MeshPtr mesh = load_mesh("cube.obj");
    ElementsPtr volume_elements = adapt(mesh);
    ElementsPtr boundary_elements = adapt_bd(mesh);
    ASSERT_EQ(12, volume_elements->getNbrElements());
    ASSERT_EQ(0, boundary_elements->getNbrElements());

    using namespace GeometryChecker;
    check_nodes(volume_elements, mesh);
    check_face_elements(volume_elements, mesh);
    check_nodes(boundary_elements, mesh);
    check_edge_elements(boundary_elements, mesh);
}

