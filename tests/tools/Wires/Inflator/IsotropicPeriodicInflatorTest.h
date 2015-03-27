/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Inflator/IsotropicPeriodicInflator.h>
#include <WireTest.h>

#include "MeshValidation.h"

class IsotropicPeriodicInflatorTest : public WireTest {
    protected:
        void inflate(const std::string& wire_file, Float thickness=0.5) {
            const Vector3F half_size(2.5, 2.5, 2.5);
            WireNetwork::Ptr network = load_wire_shared(wire_file);
            network->compute_connectivity();
            network->scale_fit(-half_size, half_size); // 5mm cell
            const size_t num_edges = network->get_num_edges();

            IsotropicPeriodicInflator inflator(network);
            inflator.set_uniform_thickness(thickness);
            inflator.inflate();

            m_vertices = inflator.get_vertices();
            m_faces = inflator.get_faces();
            m_face_sources = inflator.get_face_sources();

            ASSERT_LT(0, m_vertices.rows());
            ASSERT_LT(0, m_faces.rows());
            ASSERT_EQ(m_faces.rows(), m_face_sources.size());
        }

        void ASSERT_MESH_IS_VALID() {
            using namespace MeshValidation;
            ASSERT_TRUE(is_water_tight(m_vertices, m_faces));
            ASSERT_TRUE(is_manifold(m_vertices, m_faces));
            ASSERT_TRUE(is_periodic(m_vertices, m_faces));
            ASSERT_TRUE(face_source_is_valid(m_vertices, m_faces, m_face_sources));
        }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_face_sources;
};

TEST_F(IsotropicPeriodicInflatorTest, cube) {
    inflate("cube.wire");
    save_mesh("inflated_cube.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(IsotropicPeriodicInflatorTest, brick5) {
    inflate("brick5.wire");
    save_mesh("inflated_brick5.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(IsotropicPeriodicInflatorTest, star) {
    inflate("star_3D.wire");
    save_mesh("inflated_star.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(IsotropicPeriodicInflatorTest, diamond) {
    inflate("diamond.wire");
    save_mesh("inflated_diamond.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(IsotropicPeriodicInflatorTest, pattern1065) {
    inflate("pattern1065.wire");
    save_mesh("inflated_pattern1065.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}
