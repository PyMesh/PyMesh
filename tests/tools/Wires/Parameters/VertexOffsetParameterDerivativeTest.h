#pragma once

#include <memory>

#include <WireTest.h>
#include <Wires/Parameters/PatternParameter.h>
#include <Wires/Parameters/VertexOffsetParameter.h>
#include <Wires/Parameters/VertexOffsetParameterDerivative.h>
#include <Wires/Inflator/InflatorEngine.h>

class VertexOffsetParameterDerivativeTest : public WireTest {
    protected:
        void inflate(WireNetwork::Ptr wire_network, ParameterManager::Ptr manager) {
            InflatorEngine::Ptr inflator = InflatorEngine::create_parametric(wire_network, manager);
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

    inflate(wire_network, manager);
    ASSERT_EQ(3, m_mesh->get_dim());

    VectorF flattened_derivative = flatten(
            manager->compute_shape_velocity(m_mesh).front());
    m_mesh->add_attribute("derivative");
    m_mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("vertex_offset_derivative_brick5.msh", m_mesh, "derivative");
    ASSERT_NE(0.0, flattened_derivative.norm());

}

