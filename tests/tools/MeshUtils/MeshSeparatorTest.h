#pragma once

#include <Math/MatrixUtils.h>
#include <MeshUtils/MeshSeparator.h>
#include <TestBase.h>

class MeshSeparatorTest : public TestBase {
    protected:
        void assert_sources_are_correct(
                const MatrixIr& elements,
                const MatrixIr& component,
                const VectorI& sources) {
            ASSERT_EQ(component.rows(), sources.size());
            const size_t num_elems = component.rows();
            for (size_t i=0; i<num_elems; i++) {
                ASSERT_TRUE((component.row(i).eval().array() ==
                            elements.row(sources[i]).eval().array()).all());
            }
        }
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
    assert_sources_are_correct(faces, comp, separator.get_sources(0));
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
    assert_sources_are_correct(voxels, comp, separator.get_sources(0));
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
    assert_sources_are_correct(elements,
            separator.get_component(0), separator.get_sources(0));

    separator.set_connectivity_type(MeshSeparator::FACE);
    num_comps = separator.separate();
    ASSERT_EQ(2, num_comps);
    assert_sources_are_correct(elements,
            separator.get_component(0), separator.get_sources(0));
    assert_sources_are_correct(elements,
            separator.get_component(1), separator.get_sources(1));
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
    assert_sources_are_correct(elements,
            separator.get_component(0), separator.get_sources(0));

    separator.set_connectivity_type(MeshSeparator::FACE);
    num_comps = separator.separate();
    ASSERT_EQ(1, num_comps);
    assert_sources_are_correct(elements,
            separator.get_component(0), separator.get_sources(0));
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
    assert_sources_are_correct(elements,
            separator.get_component(0), separator.get_sources(0));

    separator.set_connectivity_type(MeshSeparator::VOXEL);
    num_comps = separator.separate();
    ASSERT_EQ(2, num_comps);
    assert_sources_are_correct(elements,
            separator.get_component(0), separator.get_sources(0));
    assert_sources_are_correct(elements,
            separator.get_component(1), separator.get_sources(1));
}
