#pragma once

#include <WireTest.h>
#include <Wires/Parameters/ParameterManager.h>
#include <Wires/Parameters/ParameterCommon.h>

class ParameterManagerTest : public WireTest {
};

TEST_F(ParameterManagerTest, creation) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(
            wire_network, 0.5);
    ASSERT_EQ(0, manager->get_num_dofs());

    ParameterCommon::Variables vars;
    VectorF thickness = manager->evaluate_thickness(vars);
    MatrixFr offset = manager->evaluate_offset(vars);

    ASSERT_EQ(wire_network->get_num_vertices(), thickness.size());
    ASSERT_EQ(wire_network->get_num_vertices(), offset.rows());
    ASSERT_EQ(wire_network->get_dim(), offset.cols());
    ASSERT_FLOAT_EQ(0.5, thickness.minCoeff());
    ASSERT_FLOAT_EQ(0.5, thickness.maxCoeff());
    ASSERT_FLOAT_EQ(0.0, offset.norm());
}

TEST_F(ParameterManagerTest, creation_from_setting) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    std::string orbit_file = m_data_dir + "brick5.orbit";
    std::string modifier_file = m_data_dir + "brick5.modifier";
    ParameterManager::Ptr manager = ParameterManager::create_from_setting_file(
            wire_network, 0.5, orbit_file, modifier_file);

    ASSERT_EQ(13, manager->get_num_dofs());
    VectorF dofs = manager->get_dofs();
    ASSERT_LT(0.0, dofs.norm());
}

TEST_F(ParameterManagerTest, add_edge_thickness) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    const size_t dim = wire_network->get_dim();
    const size_t num_vertices = wire_network->get_num_vertices();
    const size_t num_edges = wire_network->get_num_edges();

    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(
            wire_network, 0.5);

    VectorI roi(1);
    roi << 0;

    manager->set_thickness_type(ParameterCommon::EDGE);
    manager->add_thickness_parameter(roi, "test", 0.0);

    ASSERT_EQ(1, manager->get_num_dofs());
    VectorF dofs = manager->get_dofs();
    ASSERT_FLOAT_EQ(0.0, dofs.norm());

    ParameterCommon::Variables vars;
    vars["test"] = 1.1;
    VectorF thickness = manager->evaluate_thickness(vars);
    ASSERT_EQ(num_edges, thickness.size());
    ASSERT_FLOAT_EQ(1.1, thickness[0]);
    ASSERT_FLOAT_EQ(0.5, thickness.segment(1, num_edges-1).minCoeff());
    ASSERT_FLOAT_EQ(0.5, thickness.segment(1, num_edges-1).maxCoeff());

    MatrixFr offset = manager->evaluate_offset(vars);
    ASSERT_EQ(num_vertices, offset.rows());
    ASSERT_EQ(dim, offset.cols());
    ASSERT_FLOAT_EQ(0.0, offset.norm());
}

TEST_F(ParameterManagerTest, add_vertex_thickness) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t dim = wire_network->get_dim();
    const size_t num_vertices = wire_network->get_num_vertices();

    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(
            wire_network, 0.5);

    VectorI roi(4);
    roi << 0, 1, 2, 3;

    manager->set_thickness_type(ParameterCommon::VERTEX);
    manager->add_thickness_parameter(roi, "test", 0.0);

    ASSERT_EQ(1, manager->get_num_dofs());
    VectorF dofs = manager->get_dofs();
    ASSERT_FLOAT_EQ(0.0, dofs.norm());

    ParameterCommon::Variables vars;
    vars["test"] = 0.123;
    VectorF thickness = manager->evaluate_thickness(vars);
    ASSERT_EQ(num_vertices, thickness.size());
    ASSERT_FLOAT_EQ(0.123, thickness[0]);
    ASSERT_FLOAT_EQ(0.123, thickness[1]);
    ASSERT_FLOAT_EQ(0.123, thickness[2]);
    ASSERT_FLOAT_EQ(0.123, thickness[3]);
    ASSERT_FLOAT_EQ(0.5, thickness.segment(4, num_vertices-4).minCoeff());
    ASSERT_FLOAT_EQ(0.5, thickness.segment(4, num_vertices-4).maxCoeff());

    MatrixFr offset = manager->evaluate_offset(vars);
    ASSERT_EQ(num_vertices, offset.rows());
    ASSERT_EQ(dim, offset.cols());
    ASSERT_FLOAT_EQ(0.0, offset.norm());
}

TEST_F(ParameterManagerTest, add_vertex_offset) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t dim = wire_network->get_dim();
    const size_t num_vertices = wire_network->get_num_vertices();

    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(wire_network, 0.5);

    VectorI roi_0(4), roi_1(4);
    roi_0 << 0, 1, 2, 3;
    roi_1 << 4, 5, 6, 7;

    manager->set_offset_type(ParameterCommon::VERTEX);
    manager->add_offset_parameter(roi_0, "test_val0_x", 0.0, 0);
    manager->add_offset_parameter(roi_0, "test_val0_y", 0.0, 1);
    manager->add_offset_parameter(roi_0, "test_val0_z", 0.0, 2);
    manager->add_offset_parameter(roi_1, "test_val1_x", 0.0, 0);
    manager->add_offset_parameter(roi_1, "test_val1_y", 0.0, 1);
    manager->add_offset_parameter(roi_1, "test_val1_z", 0.0, 2);

    ASSERT_EQ(6, manager->get_num_dofs());
    VectorF dofs = manager->get_dofs();
    ASSERT_FLOAT_EQ(0.0, dofs.norm());

    ParameterCommon::Variables vars;
    vars["test_val0_x"] =  0.1;
    vars["test_val0_y"] =  0.2;
    vars["test_val0_z"] =  0.3;
    vars["test_val1_x"] = -0.1;
    vars["test_val1_y"] = -0.2;
    vars["test_val1_z"] = -0.3;

    MatrixFr offset = manager->evaluate_offset(vars);
    ASSERT_EQ(num_vertices, offset.rows());
    ASSERT_EQ(dim, offset.cols());
    for (size_t i=0; i<4; i++) {
        ASSERT_FLOAT_EQ(0.1, fabs(offset(i, 0)));
        ASSERT_FLOAT_EQ(0.2, fabs(offset(i, 1)));
        ASSERT_FLOAT_EQ(0.3, fabs(offset(i, 2)));
    }
    for (size_t i=4; i<8; i++) {
        ASSERT_LT(0.0, fabs(offset(i, 0)));
        ASSERT_LT(0.0, fabs(offset(i, 1)));
        ASSERT_LT(0.0, fabs(offset(i, 2)));
    }
    ASSERT_FLOAT_EQ(0.0, offset.block(8, 0, num_vertices-8, 3).norm());

    VectorF thickness = manager->evaluate_thickness(vars);
    ASSERT_FLOAT_EQ(0.5, thickness.minCoeff());
    ASSERT_FLOAT_EQ(0.5, thickness.maxCoeff());
}
