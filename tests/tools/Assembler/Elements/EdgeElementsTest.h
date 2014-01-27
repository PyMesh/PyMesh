#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/Elements/EdgeElements.h>

#include "GeometryChecker.h"

class EdgeElementsTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;
        typedef Elements::Ptr ElementsPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .create());
        }

        ElementsPtr adapt(MeshPtr mesh) {
            return ElementsPtr(new EdgeElements(mesh));
        }

    protected:
        std::string m_data_dir;
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

    /* square.obj vertex order:
     *   1---0
     *   |   |
     *   3---2
     */
    const size_t ground_truth[4][2] = {
        {1, 2},
        {0, 3},
        {0, 3},
        {1, 2}
    };

    for (size_t i=0; i<4; i++) {
        VectorI elem = square_elem->getElement(i);

        ASSERT_EQ(2, elem.size());
        ASSERT_TRUE(
                elem[1] == ground_truth[elem[0]][0] ||
                elem[1] == ground_truth[elem[0]][1]);
    }
}

