/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <WireTest.h>
#include <Wires/Parameters/ThicknessParameters.h>
#include <Wires/Parameters/ParameterCommon.h>

class ThicknessParametersTest : public WireTest {
};

TEST_F(ThicknessParametersTest, DefaultValue) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    ThicknessParameters params(wire_network, ParameterCommon::VERTEX, 0.5);
    ASSERT_FLOAT_EQ(0.5, params.get_default());
    ASSERT_EQ(wire_network, params.get_wire_network());

    ParameterCommon::Variables vars;
    VectorF thickness = params.evaluate(vars);
    ASSERT_EQ(wire_network->get_num_vertices(), thickness.size());
    ASSERT_FLOAT_EQ(0.5, thickness.minCoeff());
    ASSERT_FLOAT_EQ(0.5, thickness.maxCoeff());
}

TEST_F(ThicknessParametersTest, AddValue) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    ThicknessParameters params(wire_network, ParameterCommon::VERTEX, 0.5);
    VectorI roi(2);
    roi << 0, 1;
    params.add(roi, "", 0.1);

    ParameterCommon::Variables vars;
    VectorF thickness = params.evaluate(vars);
    ASSERT_EQ(wire_network->get_num_vertices(), thickness.size());
    ASSERT_FLOAT_EQ(0.1, thickness[0]);
    ASSERT_FLOAT_EQ(0.1, thickness[1]);
    ASSERT_TRUE((thickness.segment(2, thickness.size()-2).array() == 0.5).all());
}

TEST_F(ThicknessParametersTest, AddFormula) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    ThicknessParameters params(wire_network, ParameterCommon::VERTEX, 0.5);
    VectorI roi(2);
    roi << 0, 1;
    params.add(roi, "test", 0.1);

    ParameterCommon::Variables vars;
    vars["test"] = 1.1;
    VectorF thickness = params.evaluate(vars);
    ASSERT_EQ(wire_network->get_num_vertices(), thickness.size());
    ASSERT_FLOAT_EQ(1.1, thickness[0]);
    ASSERT_FLOAT_EQ(1.1, thickness[1]);
    ASSERT_TRUE((thickness.segment(2, thickness.size()-2).array() == 0.5).all());
}

TEST_F(ThicknessParametersTest, InvalidFormula) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    ThicknessParameters params(wire_network, ParameterCommon::VERTEX, 0.5);
    VectorI roi(2);
    roi << 0, 1;
    params.add(roi, "invalid", 0.1);

    ParameterCommon::Variables vars;
    ASSERT_THROW(params.evaluate(vars), RuntimeError);
}
