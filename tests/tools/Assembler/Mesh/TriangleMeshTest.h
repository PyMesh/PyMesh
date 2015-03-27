/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>

#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Assembler/Mesh/TriangleMesh.h>

#include <TestBase.h>

#include "BoundaryChecker.h"
#include "ConnectivityChecker.h"
#include "GeometryChecker.h"
#include "MiscChecker.h"

class TriangleMeshTest : public TestBase {
    protected:
        typedef std::shared_ptr<FEMeshAdaptor> FEMeshPtr;
        virtual void SetUp() {
            TestBase::SetUp();

            m_cube_tri = load_mesh("cube.obj");
            m_cube_tet = load_mesh("cube.msh");
            m_square = load_mesh("square_2D.obj");
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
                    .create());
        }

        FEMeshPtr adapt(MeshPtr mesh) {
            return FEMeshPtr(new TriangleMesh(mesh));
        }

    protected:
        MeshPtr m_cube_tri;
        MeshPtr m_cube_tet;
        MeshPtr m_square;
};

TEST_F(TriangleMeshTest, Dim) {
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    ASSERT_EQ(m_cube_tri->get_dim(), cube_tri->getDim());

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    ASSERT_EQ(m_cube_tet->get_dim(), cube_tet->getDim());

    FEMeshPtr square = adapt(m_square);
    ASSERT_EQ(m_square->get_dim(), square->getDim());
}

TEST_F(TriangleMeshTest, Nodes) {
    using namespace GeometryChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_nodes(cube_tri, m_cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_nodes(cube_tet, m_cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_nodes(square, m_square);
}

TEST_F(TriangleMeshTest, Elements) {
    using namespace GeometryChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_face_elements(cube_tri, m_cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_face_elements(cube_tet, m_cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_face_elements(square, m_square);
}

TEST_F(TriangleMeshTest, NodeNodeAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_node_node_adjacency_symmetry(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_node_node_adjacency_symmetry(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_node_node_adjacency_symmetry(square);
}

TEST_F(TriangleMeshTest, NodeElementAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_node_element_adjacency_symmetry(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_node_element_adjacency_symmetry(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_node_element_adjacency_symmetry(square);
}

TEST_F(TriangleMeshTest, ElementElementAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_element_element_adjacency_symmetry(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_element_element_adjacency_symmetry(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_element_element_adjacency_symmetry(square);
}

TEST_F(TriangleMeshTest, BoundaryNodeNodeAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_boundary_node_node_adjacency_symmetry(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_boundary_node_node_adjacency_symmetry(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_boundary_node_node_adjacency_symmetry(square);
}

TEST_F(TriangleMeshTest, BoundaryNodeFaceAdjacency) {
    using namespace ConnectivityChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_boundary_node_face_adjacency_symmetry(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_boundary_node_face_adjacency_symmetry(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_boundary_node_face_adjacency_symmetry(square);
}

TEST_F(TriangleMeshTest, Boundary) {
    using namespace BoundaryChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_boundary_node(cube_tri);
    check_boundary_elements(cube_tri);
    check_boundary_faces(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_boundary_node(cube_tet);
    check_boundary_elements(cube_tet);
    check_boundary_faces(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_boundary_node(square);
    check_boundary_elements(square);
    check_boundary_faces(square);
}

TEST_F(TriangleMeshTest, Normals) {
    using namespace MiscChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_normals(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_normals(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_normals(square);
}

TEST_F(TriangleMeshTest, Volume) {
    using namespace MiscChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_volumes(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_volumes(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_volumes(square);
}

TEST_F(TriangleMeshTest, Area) {
    using namespace MiscChecker;
    FEMeshPtr cube_tri = adapt(m_cube_tri);
    check_areas(cube_tri);

    FEMeshPtr cube_tet = adapt(m_cube_tet);
    check_areas(cube_tet);

    FEMeshPtr square = adapt(m_square);
    check_areas(square);
}
