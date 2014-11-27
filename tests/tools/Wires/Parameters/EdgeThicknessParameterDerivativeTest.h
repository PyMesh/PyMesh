#pragma once

#include <memory>

#include <WireTest.h>
#include <Wires/Parameters/PatternParameter.h>
#include <Wires/Parameters/ParameterCommon.h>
#include <Wires/Parameters/ParameterManager.h>
#include <Wires/Parameters/EdgeThicknessParameter.h>
#include <Wires/Parameters/EdgeThicknessParameterDerivative.h>
#include <Wires/Inflator/InflatorEngine.h>

class EdgeThicknessParameterDerivativeTest : public WireTest {
    protected:
        void inflate(WireNetwork::Ptr wire_network, ParameterManager::Ptr manager) {
            InflatorEngine::Ptr inflator = InflatorEngine::create_parametric(wire_network, manager);
            inflator->with_shape_velocities();
            inflator->inflate();

            MatrixFr vertices = inflator->get_vertices();
            MatrixIr faces = inflator->get_faces();
            VectorF face_sources = inflator->get_face_sources().cast<Float>();
            m_mesh = form_mesh(vertices, faces);
            m_mesh->add_attribute("face_source");
            m_mesh->set_attribute("face_source", face_sources);
        }

    protected:
        Mesh::Ptr m_mesh;
};

TEST_F(EdgeThicknessParameterDerivativeTest, cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(4);
    roi << 0, 7, 8, 11;

    ParameterManager::Ptr manager = ParameterManager::create_empty_manager(wire_network);
    manager->set_thickness_type(ParameterCommon::EDGE);
    manager->add_thickness_parameter(roi, "", 1.5);

    inflate(wire_network, manager);
    ASSERT_EQ(3, m_mesh->get_dim());

    VectorF flattened_derivative = flatten(
            manager->compute_shape_velocity(m_mesh).front());
    m_mesh->add_attribute("derivative");
    m_mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("edge_thickness_derivative_cube.msh", m_mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}
