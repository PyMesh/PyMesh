#pragma once

#include <iostream>

#include <Wires/Inflator/PeriodicInflator3D.h>
#include <Wires/Inflator/WireProfile.h>
#include <WireTest.h>

#include <IO/MeshWriter.h>
#include <MeshFactory.h>

#include "MeshValidation.h"

class PeriodicInflator3DTest : public WireTest {
    protected:
        void save_mesh(const std::string& file_name,
                MatrixFr& vertices, MatrixIr& faces, VectorF debug) {
            VectorF flat_vertices(vertices.rows() * vertices.cols());
            std::copy(vertices.data(), vertices.data() + flat_vertices.size(), flat_vertices.data());
            VectorI flat_faces(faces.rows() * faces.cols());
            std::copy(faces.data(), faces.data() + flat_faces.size(), flat_faces.data());
            VectorI flat_voxels = VectorI::Zero(0);

            MeshFactory factory;
            Mesh::Ptr mesh = factory.load_data(flat_vertices, flat_faces, flat_voxels,
                    vertices.cols(), faces.cols(), 4).create_shared();
            mesh->add_attribute("debug");
            mesh->set_attribute("debug", debug);

            MeshWriter::create_writer(file_name)->with_attribute("debug").write_mesh(*mesh);
        }

        void inflate(const std::string& wire_file, Float thickness=0.5) {
            const Vector3F half_size(2.5, 2.5, 2.5);
            WireNetwork network = load_wire(wire_file);
            network.compute_connectivity();
            network.scale_fit(-half_size, half_size); // 5mm cell
            const size_t num_edges = network.get_num_edges();

            PeriodicInflator3D inflator(network);
            inflator.set_thickness_type(InflatorEngine::PER_EDGE);
            inflator.set_thickness(VectorF::Ones(num_edges) * thickness);
            inflator.inflate();

            m_vertices = inflator.get_vertices();
            m_faces = inflator.get_faces();
            m_face_sources = inflator.get_face_sources();

            ASSERT_LT(0, m_vertices.rows());
            ASSERT_LT(0, m_faces.rows());
            ASSERT_EQ(m_faces.rows(), m_face_sources.size());
        }

        void inflate_with_changing_thickness(
                const std::string& wire_file, Float base_thickness=0.5,
                size_t num_profile_samples=8) {
            const Vector3F half_size(2.5, 2.5, 2.5);
            WireNetwork network = load_wire(wire_file);
            network.compute_connectivity();
            network.scale_fit(-half_size, half_size); // 5mm cell
            const size_t num_edges = network.get_num_edges();

            VectorF thickness = VectorF::Ones(num_edges) * base_thickness;
            for (size_t i=0; i<num_edges; i++) {
                thickness[i] += 0.1 * i;
            }

            WireProfile::Ptr profile = WireProfile::create_isotropic(num_profile_samples);

            PeriodicInflator3D inflator(network);
            inflator.set_thickness_type(InflatorEngine::PER_EDGE);
            inflator.set_thickness(thickness);
            inflator.set_profile(profile);
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
    inflate_with_changing_thickness("cube.wire", 0.5, 20);
    save_mesh("inflated_dense_cube.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

TEST_F(PeriodicInflator3DTest, diamond_dense) {
    inflate_with_changing_thickness("diamond.wire", 0.5, 20);
    save_mesh("inflated_dense_diamond.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
    ASSERT_MESH_IS_VALID();
}

