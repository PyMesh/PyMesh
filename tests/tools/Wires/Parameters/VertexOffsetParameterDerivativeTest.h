/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>

#include <WireTest.h>
#include <Wires/Parameters/PatternParameter.h>
#include <Wires/Parameters/VertexOffsetParameter.h>
#include <Wires/Parameters/VertexOffsetParameterDerivative.h>
#include <Wires/Inflator/InflatorEngine.h>

#include "ParameterDerivativeTest.h"

class VertexOffsetParameterDerivativeTest : public ParameterDerivativeTest {
};

TEST_F(VertexOffsetParameterDerivativeTest, brick5) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t dim = 3;
    const size_t num_wire_vertices = wire_network->get_num_vertices();
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(8);
    roi << 0, 1, 2, 3, 4, 5, 6, 7;

    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(wire_network);
    manager->set_offset_type(ParameterCommon::VERTEX);
    manager->add_offset_parameter(roi, "", 0.1, 0);

    MeshPtr mesh = inflate(wire_network, manager);
    ASSERT_EQ(3, mesh->get_dim());
    save_mesh("vertex_offset_derivative_brick5.msh", mesh,
            mesh->get_attribute_names());
}

