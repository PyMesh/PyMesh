#pragma once

#include <memory>

#include <WireTest.h>
#include <Wires/Parameters/PatternParameter.h>
#include <Wires/Parameters/EdgeThicknessParameter.h>
#include <Wires/Parameters/EdgeThicknessParameterDerivative.h>
#include <Wires/Inflator/InflatorEngine.h>

class EdgeThicknessParameterDerivativeTest : public WireTest {
    protected:
        void inflate(WireNetwork::Ptr wire_network, const VectorF& thickness) {
            InflatorEngine::Ptr inflator = InflatorEngine::create("periodic", wire_network);
            inflator->set_thickness_type(InflatorEngine::PER_EDGE);
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

TEST_F(EdgeThicknessParameterDerivativeTest, cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(4);
    roi << 0, 7, 8, 11;

    PatternParameter::Ptr param =
        std::make_shared<EdgeThicknessParameter>(wire_network);
    param->set_roi(roi);
    param->set_value(1.5);

    VectorF thickness = VectorF::Ones(wire_network->get_num_edges()) * 0.5;
    PatternParameter::Variables vars;
    param->apply(thickness, vars);
    inflate(wire_network, thickness);
    ASSERT_EQ(3, m_mesh->get_dim());

    EdgeThicknessParameterDerivative derivative(m_mesh, param);
    VectorF flattened_derivative = flatten(derivative.compute());
    m_mesh->add_attribute("derivative");
    m_mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("edge_thickness_derivative_cube.msh", m_mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}
