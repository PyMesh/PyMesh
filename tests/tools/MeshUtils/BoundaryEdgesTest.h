#pragma once

#include <algorithm>
#include <string>
#include <set>
#include <tr1/memory>

#include <Core/Exception.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <Misc/Environment.h>

#include <MeshUtils/BoundaryEdges.h>

class BoundaryEdgesTest : public ::testing::Test {
    protected:
        typedef std::tr1::shared_ptr<Mesh> MeshPtr;
        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        void load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            m_mesh = MeshPtr(MeshFactory().load_file(mesh_file).create());
        }

    protected:
        MeshPtr m_mesh;
        std::string m_data_dir;
};

TEST_F(BoundaryEdgesTest, SquareBoundary) {
    load_mesh("square_2D.obj");
    BoundaryEdges bd(*m_mesh.get());
    ASSERT_EQ(4, bd.get_num_boundaries());

    // Check each boudnary vertex lies on the boundary.
    // A vertex is on the boundary of cube if it has a coordinate
    // equals 1 or -1
    for (size_t i=0; i<4; i++) {
        VectorI face = bd.get_boundary(i);
        for (size_t j=0; j<2; j++) {
            VectorF vertex = m_mesh->get_vertex(face[j]);
            Float max_coordinate = *std::max_element(vertex.data(), vertex.data() + vertex.size());
            Float min_coordinate = *std::min_element(vertex.data(), vertex.data() + vertex.size());
            ASSERT_FLOAT_EQ(1.0, std::max(fabs(min_coordinate), fabs(max_coordinate)));
        }
    }
}

TEST_F(BoundaryEdgesTest, BoundaryElementsContainsBoundary) {
    load_mesh("square_2D.obj");
    BoundaryEdges bd(*m_mesh.get());
    const size_t num_boundaries = bd.get_num_boundaries();
    for (size_t i=0; i<num_boundaries; i++) {
        VectorI edge = bd.get_boundary(i);
        size_t face_idx = bd.get_boundary_element(i);
        VectorI face = m_mesh->get_face(face_idx);
        std::set<size_t> face_v_set(face.data(), face.data() + face.size());
        ASSERT_FALSE(face_v_set.find(edge[0]) == face_v_set.end());
        ASSERT_FALSE(face_v_set.find(edge[1]) == face_v_set.end());
    }
}

TEST_F(BoundaryEdgesTest, BoundaryNodes) {
    load_mesh("square_2D.obj");
    BoundaryEdges bd(*m_mesh.get());
    VectorI bd_nodes = bd.get_boundary_nodes();
    size_t num_bd_nodes = bd_nodes.size();

    ASSERT_EQ(4, num_bd_nodes);

    // Check that all nodes are boundary nodes.
    std::sort(bd_nodes.data(), bd_nodes.data() + num_bd_nodes);
    for (size_t i=0; i<bd_nodes.size(); i++) {
        ASSERT_EQ(i, bd_nodes[i]);
    }
}

TEST_F(BoundaryEdgesTest, NoBoundary) {
    load_mesh("tet.obj");
    BoundaryEdges bd(*m_mesh.get());
    ASSERT_EQ(0, bd.get_num_boundaries());
}

TEST_F(BoundaryEdgesTest, NoBoundaryVoxelMesh) {
    load_mesh("cube.msh");
    BoundaryEdges bd(*m_mesh.get());
    ASSERT_EQ(0, bd.get_num_boundaries());
}
