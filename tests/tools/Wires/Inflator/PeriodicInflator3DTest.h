/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <iostream>

#include <Wires/Inflator/PeriodicInflator3D.h>
#include <Wires/Inflator/WireProfile.h>
#include <Wires/Parameters/ParameterManager.h>
#include <WireTest.h>

#include <IO/MeshWriter.h>
#include <MeshFactory.h>

#include "MeshValidation.h"

class PeriodicInflator3DTest : public WireTest {
    protected:
        void inflate(const std::string& wire_file, Float thickness=0.5) {
            const Vector3F half_size(2.5, 2.5, 2.5);
            WireNetwork::Ptr network = load_wire_shared(wire_file);
            network->compute_connectivity();
            network->scale_fit(-half_size, half_size); // 5mm cell
            const size_t num_edges = network->get_num_edges();

            PeriodicInflator3D inflator(network);
            inflator.set_uniform_thickness(thickness);
            inflator.inflate();

            m_vertices = inflator.get_vertices();
            m_faces = inflator.get_faces();
            m_face_sources = inflator.get_face_sources();

            ASSERT_LT(0, m_vertices.rows());
            ASSERT_LT(0, m_faces.rows());
            ASSERT_EQ(m_faces.rows(), m_face_sources.size());
        }

        void inflate_with_circle_profile(
                const std::string& wire_file, Float base_thickness=0.5,
                size_t num_profile_samples=8) {
            const Vector3F half_size(2.5, 2.5, 2.5);
            WireNetwork::Ptr network = load_wire_shared(wire_file);
            network->compute_connectivity();
            network->scale_fit(-half_size, half_size); // 5mm cell
            const size_t num_edges = network->get_num_edges();

            WireProfile::Ptr profile = WireProfile::create_isotropic(num_profile_samples);

            InflatorEngine::Ptr inflator = InflatorEngine::create("periodic", network);
            inflator->set_uniform_thickness(base_thickness);
            inflator->set_profile(profile);
            inflator->inflate();

            m_vertices = inflator->get_vertices();
            m_faces = inflator->get_faces();
            m_face_sources = inflator->get_face_sources();

            ASSERT_LT(0, m_vertices.rows());
            ASSERT_LT(0, m_faces.rows());
            ASSERT_EQ(m_faces.rows(), m_face_sources.size());
        }

        void inflate_with_parameters(
                const std::string& wire_file,
                const std::string& orbit_file,
                const std::string& modifier_file,
                Float base_thickness=0.5) {
            const Vector3F half_size(2.5, 2.5, 2.5);
            WireNetwork::Ptr network = load_wire_shared(wire_file);
            network->compute_connectivity();
            network->scale_fit(-half_size, half_size); // 5mm cell

            ParameterManager::Ptr manager =
                ParameterManager::create_from_setting_file(
                        network, base_thickness, orbit_file, modifier_file);
            ASSERT_LT(0, manager->get_num_dofs());

            InflatorEngine::Ptr inflator = InflatorEngine::create_parametric(
                    network, manager);
            inflator->with_refinement("loop", 1);
            inflator->inflate();
            m_vertices = inflator->get_vertices();
            m_faces = inflator->get_faces();
            m_face_sources = inflator->get_face_sources();

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

TEST_F(PeriodicInflator3DTest, cube) {
    inflate("cube.wire");
    save_mesh("inflated_cube.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, brick5) {
    inflate("brick5.wire");
    save_mesh("inflated_brick5.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, star) {
    inflate("star_3D.wire");
    save_mesh("inflated_star.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, diamond) {
    inflate("diamond.wire");
    save_mesh("inflated_diamond.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, cube_dense) {
    inflate_with_circle_profile("cube.wire", 0.5, 20);
    save_mesh("inflated_dense_cube.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, diamond_dense) {
    inflate_with_circle_profile("diamond.wire", 0.5, 20);
    save_mesh("inflated_dense_diamond.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, brick5_with_params) {
    inflate_with_parameters(
            "brick5.wire",
            m_data_dir + "brick5.orbit",
            m_data_dir + "brick5.modifier",
            0.5);
    save_mesh("inflated_brick5_params.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, DISABLED_invalid) {
    ASSERT_THROW(inflate("invalid.wire"), RuntimeError);
}

