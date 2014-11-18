#pragma once

#include <memory>

#include <WireTest.h>
#include <Wires/Parameters/PatternParameter.h>
#include <Wires/Parameters/VertexOffsetParameter.h>
#include <Wires/Parameters/VertexOffsetParameterDerivative.h>
#include <Wires/Inflator/InflatorEngine.h>

class VertexOffsetParameterDerivativeTest : public WireTest {
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

TEST_F(VertexOffsetParameterDerivativeTest, brick5) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    const size_t dim = 3;
    const size_t num_wire_vertices = wire_network->get_num_vertices();
    wire_network->scale(Vector3F::Ones() * 2.5);
    VectorI roi(8);
    roi << 0, 1, 2, 3, 4, 5, 6, 7;

    PatternParameter::Ptr param =
        std::make_shared<VertexOffsetParameter>(wire_network, 0);
    param->set_roi(roi);
    param->set_value(0.1);

    VectorF thickness = VectorF::Ones(num_wire_vertices) * 0.5;
    VectorF flattened_offset = VectorF::Zero(num_wire_vertices * dim);
    PatternParameter::Variables vars;
    param->apply(flattened_offset, vars);
    MatrixFr offset = Eigen::Map<MatrixFr>(
            flattened_offset.data(), num_wire_vertices, dim);

    wire_network->set_vertices(wire_network->get_vertices() + offset);
    inflate(wire_network, thickness);
    wire_network->set_vertices(wire_network->get_vertices() - offset);
    ASSERT_EQ(dim, m_mesh->get_dim());

    VertexOffsetParameterDerivative derivative(m_mesh, param);
    VectorF flattened_derivative = flatten(derivative.compute());
    m_mesh->add_attribute("derivative");
    m_mesh->set_attribute("derivative", flattened_derivative);

    save_mesh("vertex_offset_derivative_brick5.msh", m_mesh, "derivative");

    ASSERT_NE(0.0, flattened_derivative.norm());
}

