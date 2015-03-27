/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <algorithm>
#include <string>
#include <set>

#include <Core/Exception.h>

#include <MeshUtils/BoundaryEdges.h>

#include <TestBase.h>

class BoundaryEdgesTest : public TestBase {
};

TEST_F(BoundaryEdgesTest, SquareBoundary) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    BoundaryEdges bd(*mesh.get());
    ASSERT_EQ(4, bd.get_num_boundaries());

    // Check each boudnary vertex lies on the boundary.
    // A vertex is on the boundary of cube if it has a coordinate
    // equals 1 or -1
    for (size_t i=0; i<4; i++) {
        VectorI face = bd.get_boundary(i);
        for (size_t j=0; j<2; j++) {
            VectorF vertex = mesh->get_vertex(face[j]);
            Float max_coordinate = *std::max_element(vertex.data(), vertex.data() + vertex.size());
            Float min_coordinate = *std::min_element(vertex.data(), vertex.data() + vertex.size());
            ASSERT_FLOAT_EQ(1.0, std::max(fabs(min_coordinate), fabs(max_coordinate)));
        }
    }
}

TEST_F(BoundaryEdgesTest, BoundaryElementsContainsBoundary) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    BoundaryEdges bd(*mesh.get());
    const size_t num_boundaries = bd.get_num_boundaries();
    for (size_t i=0; i<num_boundaries; i++) {
        VectorI edge = bd.get_boundary(i);
        size_t face_idx = bd.get_boundary_element(i);
        VectorI face = mesh->get_face(face_idx);
        std::set<size_t> face_v_set(face.data(), face.data() + face.size());
        ASSERT_FALSE(face_v_set.find(edge[0]) == face_v_set.end());
        ASSERT_FALSE(face_v_set.find(edge[1]) == face_v_set.end());
    }
}

TEST_F(BoundaryEdgesTest, BoundaryNodes) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    BoundaryEdges bd(*mesh.get());
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
    MeshPtr mesh = load_mesh("tet.obj");
    BoundaryEdges bd(*mesh.get());
    ASSERT_EQ(0, bd.get_num_boundaries());
}

TEST_F(BoundaryEdgesTest, NoBoundaryVoxelMesh) {
    MeshPtr mesh = load_mesh("cube.msh");
    BoundaryEdges bd(*mesh.get());
    ASSERT_EQ(0, bd.get_num_boundaries());
}
