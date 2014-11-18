#pragma once

#include <memory>
#include <iostream>

#include <Mesh.h>
#include <WireTest.h>
#include <Wires/Parameters/PatternParameter.h>
#include <Wires/Parameters/VertexThicknessParameter.h>
#include <Wires/Parameters/VertexThicknessParameterDerivative.h>
#include <Wires/Inflator/InflatorEngine.h>

class VertexThicknessParameterDerivativeTest : public WireTest {
    protected:
        void inflate(WireNetwork::Ptr wire_network, const VectorF& thickness) {
            InflatorEngine::Ptr inflator = InflatorEngine::create("periodic", wire_network);
            inflator->set_thickness_type(InflatorEngine::PER_VERTEX);
            inflator->set_thickness(thickness);
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

TEST_F(VertexThicknessParameterDerivativeTest, cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(8);
    roi << 0, 1, 2, 3, 4, 5, 6, 7;

    PatternParameter::Ptr param =
        std::make_shared<VertexThicknessParameter>(wire_network);
    param->set_roi(roi);
    param->set_value(1.5);

    VectorF thickness = VectorF::Ones(wire_network->get_num_vertices()) * 0.5;
    PatternParameter::Variables vars;
    param->apply(thickness, vars);
    inflate(wire_network, thickness);
    ASSERT_EQ(3, m_mesh->get_dim());

    VertexThicknessParameterDerivative derivative(m_mesh, param);
    VectorF flattened_derivative = flatten(derivative.compute());
    m_mesh->add_attribute("derivative");
    m_mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("vertex_thickness_derivative_cube.msh", m_mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}

TEST_F(VertexThicknessParameterDerivativeTest, brick5) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(8);
    roi << 0, 1, 2, 3, 4, 5, 6, 7;

    PatternParameter::Ptr param =
        std::make_shared<VertexThicknessParameter>(wire_network);
    param->set_roi(roi);
    param->set_value(1.5);

    VectorF thickness = VectorF::Ones(wire_network->get_num_vertices()) * 0.5;
    PatternParameter::Variables vars;
    param->apply(thickness, vars);
    inflate(wire_network, thickness);
    ASSERT_EQ(3, m_mesh->get_dim());

    VertexThicknessParameterDerivative derivative(m_mesh, param);
    VectorF flattened_derivative = flatten(derivative.compute());
    m_mesh->add_attribute("derivative");
    m_mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("vertex_thickness_derivative_brick5.msh", m_mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}

TEST_F(VertexThicknessParameterDerivativeTest, DISABLED_2D) {
    WireNetwork::Ptr wire_network = load_wire_shared("box.wire");
    wire_network->scale(Vector2F::Ones() * 5);
    VectorI roi(4);
    roi << 0, 1, 2, 3;

    PatternParameter::Ptr param =
        std::make_shared<VertexThicknessParameter>(wire_network);
    param->set_roi(roi);
    param->set_value(1.5);

    VectorF thickness = VectorF::Ones(wire_network->get_num_vertices()) * 0.5;
    PatternParameter::Variables vars;
    param->apply(thickness, vars);
    inflate(wire_network, thickness);
    ASSERT_EQ(2, m_mesh->get_dim());

    VertexThicknessParameterDerivative derivative(m_mesh, param);
    VectorF flattened_derivative = flatten(derivative.compute());
    m_mesh->add_attribute("derivative");
    m_mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("thickness_derivative_2D.msh", m_mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}
