/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>

#include <Core/Exception.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/Mesh/TetrahedronMesh.h>

#include <TestBase.h>

#include "BoundaryChecker.h"
#include "ConnectivityChecker.h"
#include "GeometryChecker.h"
#include "MiscChecker.h"

class TetrahedronMeshTest : public TestBase {
    protected:
        typedef std::shared_ptr<FEMeshAdaptor> FEMeshPtr;
        virtual void SetUp() {
            TestBase::SetUp();
            m_cube_tet = load_mesh("cube.msh");
        }

        virtual MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .with_connectivity("all")
                    .with_attribute("face_normal")
                    .with_attribute("vertex_normal")
                    .with_attribute("face_area")
                    .with_attribute("voxel_volume")
                    .create());
        }

        FEMeshPtr adapt(MeshPtr mesh) {
            return FEMeshPtr(new TetrahedronMesh(mesh));
        }

    protected:
        MeshPtr m_cube_tet;
};

TEST_F(TetrahedronMeshTest, NonVoxelMesh) {
    MeshPtr cube = load_mesh("cube.obj");
    ASSERT_THROW(adapt(cube), RuntimeError);
}

TEST_F(TetrahedronMeshTest, Dim) {
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    ASSERT_EQ(m_cube_tet->get_dim(), cube_tet->getDim());
}

TEST_F(TetrahedronMeshTest, Nodes) {
    using namespace GeometryChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_nodes(cube_tet, m_cube_tet);
}

TEST_F(TetrahedronMeshTest, Elements) {
    using namespace GeometryChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_voxel_elements(cube_tet, m_cube_tet);
}

TEST_F(TetrahedronMeshTest, NodeNodeAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_node_node_adjacency_symmetry(cube_tet);
}

TEST_F(TetrahedronMeshTest, NodeElementAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_node_element_adjacency_symmetry(cube_tet);
}

TEST_F(TetrahedronMeshTest, ElementElementAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_element_element_adjacency_symmetry(cube_tet);
}

TEST_F(TetrahedronMeshTest, BoundaryNodeNodeAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_boundary_node_node_adjacency_symmetry(cube_tet);
}

TEST_F(TetrahedronMeshTest, BoundaryNodeFaceAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_boundary_node_face_adjacency_symmetry(cube_tet);
}

TEST_F(TetrahedronMeshTest, Boundary) {
    using namespace BoundaryChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_boundary_node(cube_tet);
    check_boundary_elements(cube_tet);
    check_boundary_faces(cube_tet);
}

TEST_F(TetrahedronMeshTest, Normals) {
    using namespace MiscChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_normals(cube_tet);
}

TEST_F(TetrahedronMeshTest, Volume) {
    using namespace MiscChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_volumes(cube_tet);
}

TEST_F(TetrahedronMeshTest, Area) {
    using namespace MiscChecker;
    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_areas(cube_tet);
}
