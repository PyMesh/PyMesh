/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <iostream>

#include <Mesh.h>
#include <WireTest.h>
#include <Wires/Parameters/PatternParameter.h>
#include <Wires/Parameters/VertexThicknessParameter.h>
#include <Wires/Parameters/VertexThicknessParameterDerivative.h>
#include <Wires/Inflator/InflatorEngine.h>

#include "ParameterDerivativeTest.h"

class VertexThicknessParameterDerivativeTest : public ParameterDerivativeTest {
};

TEST_F(VertexThicknessParameterDerivativeTest, cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(8);
    roi << 0, 1, 2, 3, 4, 5, 6, 7;

    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(wire_network);
    manager->set_thickness_type(ParameterCommon::VERTEX);
    manager->add_thickness_parameter(roi, "", 1.5);

    Mesh::Ptr mesh = inflate(wire_network, manager);
    ASSERT_EQ(3, mesh->get_dim());

    VectorF flattened_derivative = flatten(
            manager->compute_shape_velocity(mesh).front());
    mesh->add_attribute("derivative");
    mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("vertex_thickness_derivative_cube.msh", mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}

TEST_F(VertexThicknessParameterDerivativeTest, brick5) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(8);
    roi << 0, 1, 2, 3, 4, 5, 6, 7;

    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(wire_network);
    manager->set_thickness_type(ParameterCommon::VERTEX);
    manager->add_thickness_parameter(roi, "", 1.5);

    Mesh::Ptr mesh = inflate(wire_network, manager);
    ASSERT_EQ(3, mesh->get_dim());

    VectorF flattened_derivative = flatten(
            manager->compute_shape_velocity(mesh).front());
    mesh->add_attribute("derivative");
    mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("vertex_thickness_derivative_brick5.msh", mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}
