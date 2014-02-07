#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <Assembler/Elements/Elements.h>
#include "GeometryChecker.h"

class ElementsTest : public ::testing::Test {
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
            return Elements::adapt(mesh);
        }

        ElementsPtr adapt_bd(MeshPtr mesh) {
            return Elements::adapt_boundary(mesh);
        }

    protected:
        std::string m_data_dir;
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

