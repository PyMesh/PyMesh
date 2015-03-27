/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <WireTest.h>
#include <Wires/Parameters/OffsetParameters.h>
#include <Wires/Parameters/ParameterCommon.h>

class OffsetParameterTest : public WireTest {
};

TEST_F(OffsetParameterTest, DefaultValue) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t dim = wire_network->get_dim();
    const size_t num_vertices = wire_network->get_num_vertices();

    OffsetParameters params(wire_network, ParameterCommon::VERTEX, 0.0);
    ASSERT_FLOAT_EQ(0.0, params.get_default());
    ASSERT_EQ(wire_network, params.get_wire_network());

    ParameterCommon::Variables vars;
    MatrixFr offset = params.evaluate(vars);
    ASSERT_EQ(num_vertices, offset.rows());
    ASSERT_EQ(dim, offset.cols());
    ASSERT_FLOAT_EQ(0.0, offset.minCoeff());
    ASSERT_FLOAT_EQ(0.0, offset.maxCoeff());
}

TEST_F(OffsetParameterTest, AddValue) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t dim = wire_network->get_dim();
    const size_t num_vertices = wire_network->get_num_vertices();

    OffsetParameters params(wire_network, ParameterCommon::VERTEX, 0.0);
    ASSERT_FLOAT_EQ(0.0, params.get_default());
    ASSERT_EQ(wire_network, params.get_wire_network());

    // This roi is carefully chosen to exclude any boundary vertices.
    // All vertices of this roi lies on a unit box.
    VectorI roi(4);
    roi << 0, 1, 2, 3;
    params.add(roi, "", 0.1, 0);
    params.add(roi, "", 0.1, 1);
    params.add(roi, "", 0.1, 2);

    const VectorF bbox_half_size =
        wire_network->get_bbox_max() - wire_network->center();
    const Float half_size = bbox_half_size[0];

    ParameterCommon::Variables vars;
    MatrixFr offset = params.evaluate(vars);
    ASSERT_EQ(num_vertices, offset.rows());
    ASSERT_EQ(dim, offset.cols());
    ASSERT_TRUE((offset.block(0, 0, 4, dim).cwiseAbs().array() == 0.1 * half_size).all());
    ASSERT_TRUE((offset.block(4, 0, offset.rows() - 4, dim).array() == 0.0)
            .all());
}

TEST_F(OffsetParameterTest, AddFormula) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t dim = wire_network->get_dim();
    const size_t num_vertices = wire_network->get_num_vertices();
    const VectorF bbox_half_size =
        wire_network->get_bbox_max() - wire_network->center();

    OffsetParameters params(wire_network, ParameterCommon::VERTEX, 0.0);

    // This roi is carefully chosen to exclude any boundary vertices.
    // All vertices of this roi lies on a unit box.
    VectorI roi(4);
    roi << 0, 1, 2, 3;
    params.add(roi, "x", 0.0, 0);
    params.add(roi, "y", 0.0, 1);
    params.add(roi, "z", 0.0, 2);

    ParameterCommon::Variables vars;
    vars["x"] = 0.1;
    vars["y"] = 0.2;
    vars["z"] = 0.3;
    MatrixFr offset = params.evaluate(vars);
    ASSERT_EQ(num_vertices, offset.rows());
    ASSERT_EQ(dim, offset.cols());
    for (size_t i=0; i<4; i++) {
        const VectorF& u = offset.row(i);
        ASSERT_FLOAT_EQ(0.1 * bbox_half_size[0], fabs(u[0]));
        ASSERT_FLOAT_EQ(0.2 * bbox_half_size[1], fabs(u[1]));
        ASSERT_FLOAT_EQ(0.3 * bbox_half_size[2], fabs(u[2]));
    }
    ASSERT_TRUE((offset.block(4, 0, offset.rows() - 4, dim).array() == 0.0)
            .all());
}

TEST_F(OffsetParameterTest, InvalidDof) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    OffsetParameters params(wire_network, ParameterCommon::VERTEX, 0.0);

    // Since all vertices of cube are on boundary, no matter what roi is, there
    // is no degree of freedom for offseting them.
    VectorI roi(2);
    roi << 0, 2;

    params.add(roi, "", 1.0, 0);

    ASSERT_EQ(0, params.get_num_dofs());
    ParameterCommon::Variables vars;
    MatrixFr offset = params.evaluate(vars);
    ASSERT_FLOAT_EQ(0.0, offset.maxCoeff());
    ASSERT_FLOAT_EQ(0.0, offset.minCoeff());
}
