/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Inflator/SimpleInflator.h>
#include <Wires/Inflator/WireProfile.h>
#include <Wires/Parameters/ParameterCommon.h>
#include <Wires/Parameters/ParameterManager.h>
#include <Wires/Tiler/AABBTiler.h>
#include <WireTest.h>
#include <IO/MeshWriter.h>

class SimpleInflatorTest : public WireTest {
};

TEST_F(SimpleInflatorTest, 2D) {
    WireNetwork::Ptr network = load_wire_shared("square.wire");
    network->compute_connectivity();
    const size_t num_edges = network->get_num_edges();

    SimpleInflator inflator(network);
    inflator.set_thickness_type(SimpleInflator::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.2);
    inflator.with_refinement("loop", 2);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(faces.rows(), face_sources.size());

    //save_mesh("tmp.obj", vertices, faces);
}

TEST_F(SimpleInflatorTest, 2D_tile_with_parameters) {
    WireNetwork::Ptr network = load_wire_shared("box.wire");
    network->compute_connectivity();

    ParameterManager::Ptr manager = ParameterManager::create_from_setting_file(
            network, 0.5, m_data_dir + "box.orbit", m_data_dir + "box.modifier");

    Vector2F bbox_min(0.0, 0.0);
    Vector2F bbox_max(10.0, 10.0);
    Vector2I repetitions(2, 2);
    AABBTiler tiler(network, bbox_min, bbox_max, repetitions);
    tiler.with_parameters(manager);

    WireNetwork::Ptr tiled_network = tiler.tile();
    MatrixFr offset = tiled_network->get_attribute("vertex_offset");
    MatrixFr thickness = tiled_network->get_attribute("thickness");
    tiled_network->set_vertices(tiled_network->get_vertices() + offset);

    SimpleInflator inflator(tiled_network);
    if (manager->get_thickness_type() == ParameterCommon::VERTEX) {
        inflator.set_thickness_type(SimpleInflator::PER_VERTEX);
    } else {
        inflator.set_thickness_type(SimpleInflator::PER_EDGE);
    }
    inflator.set_thickness(thickness);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(faces.rows(), face_sources.size());

    //save_mesh("tmp.obj", vertices, faces);
}

TEST_F(SimpleInflatorTest, 3D) {
    WireNetwork::Ptr network = load_wire_shared("cube.wire");
    network->compute_connectivity();
    const size_t num_edges = network->get_num_edges();

    WireProfile::Ptr profile = WireProfile::create("hexagon");

    SimpleInflator inflator(network);
    inflator.set_profile(profile);
    inflator.set_thickness_type(SimpleInflator::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.25);
    inflator.with_refinement("loop", 2);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(faces.rows(), face_sources.size());

    //save_mesh("tmp.obj", vertices, faces);
}

TEST_F(SimpleInflatorTest, 3D_tile_with_parameters) {
    WireNetwork::Ptr network = load_wire_shared("brick5.wire");
    network->compute_connectivity();

    ParameterManager::Ptr manager = ParameterManager::create_from_setting_file(
            network, 0.5, m_data_dir + "brick5.orbit", m_data_dir + "brick5.modifier");

    Vector3F bbox_min(0.0, 0.0, 0.0);
    Vector3F bbox_max(10.0, 10.0, 10.0);
    Vector3I repetitions(2, 2, 2);
    AABBTiler tiler(network, bbox_min, bbox_max, repetitions);
    tiler.with_parameters(manager);

    WireNetwork::Ptr tiled_network = tiler.tile();
    MatrixFr offset = tiled_network->get_attribute("vertex_offset");
    MatrixFr thickness = tiled_network->get_attribute("thickness");
    tiled_network->set_vertices(tiled_network->get_vertices() + offset);

    SimpleInflator inflator(tiled_network);
    if (manager->get_thickness_type() == ParameterCommon::VERTEX) {
        inflator.set_thickness_type(SimpleInflator::PER_VERTEX);
    } else {
        inflator.set_thickness_type(SimpleInflator::PER_EDGE);
    }
    inflator.set_thickness(thickness);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(faces.rows(), face_sources.size());

    save_mesh("tmp.obj", vertices, faces);
}

