/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <WireTest.h>
#include <Wires/Parameters/ParameterManager.h>
#include <Wires/Parameters/ParameterCommon.h>

class ParameterManagerTest : public WireTest {
};

TEST_F(ParameterManagerTest, creation_default_cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(1, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(3, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, creation_default_brick5) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(7, manager->get_num_thickness_dofs());
    ASSERT_EQ(6, manager->get_num_offset_dofs());
    ASSERT_EQ(13, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(12, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, creation_default_square) {
    WireNetwork::Ptr wire_network = load_wire_shared("square.wire");
    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(1, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(2, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, creation_default_box) {
    WireNetwork::Ptr wire_network = load_wire_shared("box.wire");
    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(9, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(8, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, create_isotropic_cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    ParameterManager::Ptr manager = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(1, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(1, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, creation_isotropic_brick5) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    ParameterManager::Ptr manager = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(3, manager->get_num_thickness_dofs());
    ASSERT_EQ(2, manager->get_num_offset_dofs());
    ASSERT_EQ(5, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(4, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, creation_isotropic_square) {
    WireNetwork::Ptr wire_network = load_wire_shared("square.wire");
    ParameterManager::Ptr manager = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(1, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(1, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, creation_isotropic_box) {
    WireNetwork::Ptr wire_network = load_wire_shared("box.wire");
    ParameterManager::Ptr manager = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::VERTEX);
    ASSERT_EQ(3, manager->get_num_thickness_dofs());
    ASSERT_EQ(2, manager->get_num_offset_dofs());
    ASSERT_EQ(5, manager->get_num_dofs());

    ParameterManager::Ptr manager2 = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::EDGE);
    ASSERT_EQ(4, manager2->get_num_dofs());
}

TEST_F(ParameterManagerTest, creation_empty) {
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

TEST_F(ParameterManagerTest, create_from_dof_file_cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    std::string dof_file = m_data_dir + "cube.dof";
    ParameterManager::Ptr manager = ParameterManager::create_from_dof_file(
            wire_network, 0.5, dof_file);

    ASSERT_EQ(1, manager->get_num_dofs());
    VectorF dofs = manager->get_dofs();
    ASSERT_FLOAT_EQ(0.75, dofs[0]);
}

TEST_F(ParameterManagerTest, create_from_dof_file_brick5) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    std::string dof_file = m_data_dir + "brick5.dof";
    ParameterManager::Ptr manager = ParameterManager::create_from_dof_file(
            wire_network, 0.5, dof_file);

    ASSERT_EQ(4, manager->get_num_dofs());
    VectorF dofs = manager->get_dofs();
    VectorF expected_dofs(4);
    expected_dofs << 0.1, 0.1, 0.2, 0.2;
    ASSERT_FLOAT_EQ(0.0, (dofs - expected_dofs).norm());
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
    for (size_t i=0; i<8; i++) {
        ASSERT_LT(0.0, fabs(offset(i, 0)));
        ASSERT_LT(0.0, fabs(offset(i, 1)));
        ASSERT_LT(0.0, fabs(offset(i, 2)));
    }
    ASSERT_FLOAT_EQ(0.0, offset.block(8, 0, num_vertices-8, 3).norm());

    VectorF thickness = manager->evaluate_thickness(vars);
    ASSERT_FLOAT_EQ(0.5, thickness.minCoeff());
    ASSERT_FLOAT_EQ(0.5, thickness.maxCoeff());
}

TEST_F(ParameterManagerTest, vertex_thickness_dof_map) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t num_vertices = wire_network->get_num_vertices();

    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::VERTEX);
    VectorI thickness_dof_map = manager->get_thickness_dof_map();

    ASSERT_EQ(num_vertices, thickness_dof_map.size());

    ParameterCommon::Variables vars;
    const size_t num_thickness_dof = manager->get_num_thickness_dofs();
    for (size_t i=0; i<num_thickness_dof; i++) {
        VectorF cur_dofs = manager->get_dofs();
        VectorF cur_thickness = manager->evaluate_thickness(vars);

        VectorF new_dofs = cur_dofs;
        new_dofs[i] += 0.1;
        manager->set_dofs(new_dofs);
        VectorF new_thickness = manager->evaluate_thickness(vars);

        VectorF diff_thickness = new_thickness - cur_thickness;
        for (size_t j=0; j<num_vertices; j++) {
            if (thickness_dof_map[j] != i) {
                ASSERT_FLOAT_EQ(0.0, diff_thickness[j]);
            } else {
                ASSERT_LT(0.0, diff_thickness[j]);
            }
        }
    }
}

TEST_F(ParameterManagerTest, edge_thickness_dof_map) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t num_edges = wire_network->get_num_edges();

    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::EDGE);
    VectorI thickness_dof_map = manager->get_thickness_dof_map();

    ASSERT_EQ(num_edges, thickness_dof_map.size());

    ParameterCommon::Variables vars;
    const size_t num_thickness_dof = manager->get_num_thickness_dofs();
    for (size_t i=0; i<num_thickness_dof; i++) {
        VectorF cur_dofs = manager->get_dofs();
        VectorF cur_thickness = manager->evaluate_thickness(vars);

        VectorF new_dofs = cur_dofs;
        new_dofs[i] += 0.1;
        manager->set_dofs(new_dofs);
        VectorF new_thickness = manager->evaluate_thickness(vars);

        VectorF diff_thickness = new_thickness - cur_thickness;
        for (size_t j=0; j<num_edges; j++) {
            if (thickness_dof_map[j] != i) {
                ASSERT_FLOAT_EQ(0.0, diff_thickness[j]);
            } else {
                ASSERT_LT(0.0, diff_thickness[j]);
            }
        }
    }
}

TEST_F(ParameterManagerTest, vertex_offset_dof_map) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::VERTEX);

    MatrixIr offset_dof_map = manager->get_offset_dof_map();

    const size_t dim = wire_network->get_dim();
    const size_t num_vertices = wire_network->get_num_vertices();
    ASSERT_EQ(num_vertices, offset_dof_map.rows());
    ASSERT_EQ(dim, offset_dof_map.cols());

    ParameterCommon::Variables vars;
    const size_t num_thickness_dof = manager->get_num_thickness_dofs();
    const size_t num_dofs = manager->get_num_dofs();
    for (size_t i=num_thickness_dof; i<num_dofs; i++) {
        VectorF cur_dofs = manager->get_dofs();
        MatrixFr cur_offset = manager->evaluate_offset(vars);

        VectorF new_dofs = cur_dofs;
        new_dofs[i] += 0.1;
        manager->set_dofs(new_dofs);
        MatrixFr new_offset = manager->evaluate_offset(vars);

        MatrixFr diff_offset = new_offset - cur_offset;
        for (size_t j=0; j<num_vertices; j++) {
            for (size_t k=0; k<dim; k++) {
                if (offset_dof_map(j,k) != i) {
                    ASSERT_FLOAT_EQ(0.0, diff_offset(j,k));
                } else {
                    ASSERT_LT(0.0, fabs(diff_offset(j,k)));
                }
            }
        }
    }
}

TEST_F(ParameterManagerTest, save_dofs) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    ParameterManager::Ptr manager = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::VERTEX);
    VectorF dofs = manager->get_dofs();
    for (size_t i=0; i<dofs.size(); i++) {
        dofs[i] = 0.1 + 0.02 * i;
    }
    manager->set_dofs(dofs);
    manager->save_dofs("tmp.dof");

    ParameterManager::Ptr manager2 =
        ParameterManager::create_from_dof_file(wire_network, 0.5, "tmp.dof");
    VectorF dofs2 = manager2->get_dofs();

    ASSERT_EQ(dofs.size(), dofs2.size());
    ASSERT_NEAR(0.0, (dofs-dofs2).norm(), 1e-12);
}

TEST_F(ParameterManagerTest, constant_isotropic_offset_wire_grad) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    ParameterManager::Ptr manager = ParameterManager::create_isotropic(
            wire_network, 0.5, ParameterCommon::VERTEX);
    const size_t num_dofs = manager->get_num_dofs();
    const size_t num_offset_dofs = manager->get_num_offset_dofs();
    VectorF dofs = manager->get_dofs();

    std::vector<MatrixFr> offset_wire_grads;
    for (const auto& param : manager->get_offset_params()) {
        offset_wire_grads.emplace_back(param->compute_derivative());
    }

    dofs += VectorF::Ones(num_dofs) * 0.1;
    manager->set_dofs(dofs);
    MatrixFr offset = manager->evaluate_offset_no_formula();
    wire_network->set_vertices(wire_network->get_vertices() + offset);

    std::vector<MatrixFr> updated_offset_wire_grads;
    for (const auto& param : manager->get_offset_params()) {
        updated_offset_wire_grads.emplace_back(param->compute_derivative());
    }

    ASSERT_EQ(num_offset_dofs, offset_wire_grads.size());
    ASSERT_EQ(num_offset_dofs, updated_offset_wire_grads.size());
    for (size_t i=0; i<num_offset_dofs; i++) {
        Float diff = (offset_wire_grads[i] - updated_offset_wire_grads[i])
            .squaredNorm();
        ASSERT_FLOAT_EQ(0.0, diff);
    }
}

TEST_F(ParameterManagerTest, constant_anisotropic_offset_wire_grad) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    ParameterManager::Ptr manager = ParameterManager::create(
            wire_network, 0.5, ParameterCommon::VERTEX);
    const size_t num_dofs = manager->get_num_dofs();
    const size_t num_offset_dofs = manager->get_num_offset_dofs();
    VectorF dofs = manager->get_dofs();

    std::vector<MatrixFr> offset_wire_grads;
    for (const auto& param : manager->get_offset_params()) {
        offset_wire_grads.emplace_back(param->compute_derivative());
    }

    dofs += VectorF::Ones(num_dofs) * 0.1;
    manager->set_dofs(dofs);
    MatrixFr offset = manager->evaluate_offset_no_formula();
    wire_network->set_vertices(wire_network->get_vertices() + offset);

    std::vector<MatrixFr> updated_offset_wire_grads;
    for (const auto& param : manager->get_offset_params()) {
        updated_offset_wire_grads.emplace_back(param->compute_derivative());
    }

    ASSERT_EQ(num_offset_dofs, offset_wire_grads.size());
    ASSERT_EQ(num_offset_dofs, updated_offset_wire_grads.size());
    for (size_t i=0; i<num_offset_dofs; i++) {
        Float diff = (offset_wire_grads[i] - updated_offset_wire_grads[i])
            .squaredNorm();
        ASSERT_FLOAT_EQ(0.0, diff);
    }
}
