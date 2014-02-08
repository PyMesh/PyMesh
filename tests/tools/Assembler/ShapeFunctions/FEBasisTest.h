#pragma once

#include <string>

#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <Assembler/Elements/Elements.h>
#include <Assembler/ShapeFunctions/FEBasis.h>

class FEBasisTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;
        typedef Elements::Ptr FEMeshPtr;
        void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        FEMeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return Elements::adapt(
                    MeshPtr(
                        MeshFactory()
                        .load_file(mesh_file)
                        .create()));
        }

    protected:
        std::string m_data_dir;
};

TEST_F(FEBasisTest, Creation) {
    FEMeshPtr cube_tet = load_mesh("cube.msh");
    //FEMeshPtr cube_tri = load_mesh("cube.obj");
    FEMeshPtr square = load_mesh("square_2D.obj");

    FEBasis cube_tet_basis = FEBasis(cube_tet);
    //FEBasis cube_tri_basis = FEBasis(cube_tri);
    FEBasis square_basis = FEBasis(square);
}
