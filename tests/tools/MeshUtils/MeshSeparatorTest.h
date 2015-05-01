#pragma once

#include <Math/MatrixUtils.h>
#include <MeshUtils/MeshSeparator.h>
#include <TestBase.h>

class MeshSeparatorTest : public TestBase {
    protected:
};

TEST_F(MeshSeparatorTest, single_surface_component) {
    Mesh::Ptr mesh = load_mesh("cube.obj");
    const auto faces = MatrixUtils::reshape<MatrixIr>(mesh->get_faces(),
            mesh->get_num_faces(), mesh->get_vertex_per_face());
    MeshSeparator separator(faces);
    size_t num_comps = separator.separate();
    ASSERT_EQ(1, num_comps);

    auto comp = separator.get_component(0);
    ASSERT_EQ(mesh->get_num_faces(), comp.rows());
}

TEST_F(MeshSeparatorTest, single_volume_component) {
    Mesh::Ptr mesh = load_mesh("cube.msh");
    const auto voxels = MatrixUtils::reshape<MatrixIr>(mesh->get_voxels(),
            mesh->get_num_voxels(), mesh->get_vertex_per_voxel());
    MeshSeparator separator(voxels);
    separator.set_connectivity_type(MeshSeparator::VOXEL);
    size_t num_comps = separator.separate();
    ASSERT_EQ(1, num_comps);

    auto comp = separator.get_component(0);
    ASSERT_EQ(mesh->get_num_voxels(), comp.rows());
}

TEST_F(MeshSeparatorTest, vertex_connected) {
    MatrixIr elements(3, 3);
    elements << 0, 1, 2,
                0, 1, 3,
                0, 4, 5;

    MeshSeparator separator(elements);
    separator.set_connectivity_type(MeshSeparator::VERTEX);
    size_t num_comps = separator.separate();
    ASSERT_EQ(1, num_comps);

    separator.set_connectivity_type(MeshSeparator::FACE);
    num_comps = separator.separate();
    ASSERT_EQ(2, num_comps);
}

TEST_F(MeshSeparatorTest, face_connected) {
    MatrixIr elements(3, 3);
    elements << 0, 1, 2,
                0, 1, 3,
                1, 2, 3;

    MeshSeparator separator(elements);
    separator.set_connectivity_type(MeshSeparator::VERTEX);
    size_t num_comps = separator.separate();
    ASSERT_EQ(1, num_comps);

    separator.set_connectivity_type(MeshSeparator::FACE);
    num_comps = separator.separate();
    ASSERT_EQ(1, num_comps);
}

TEST_F(MeshSeparatorTest, voxel_connected) {
    MatrixIr elements(3, 4);
    elements << 0, 1, 2, 3,
                0, 1, 3, 4,
                1, 2, 6, 5;

    MeshSeparator separator(elements);
    separator.set_connectivity_type(MeshSeparator::VERTEX);
    size_t num_comps = separator.separate();
    ASSERT_EQ(1, num_comps);

    separator.set_connectivity_type(MeshSeparator::VOXEL);
    num_comps = separator.separate();
    ASSERT_EQ(2, num_comps);
}
