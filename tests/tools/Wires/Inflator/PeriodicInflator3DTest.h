#pragma once

#include <iostream>

#include <Wires/Inflator/PeriodicInflator3D.h>
#include <WireTest.h>

#include <IO/MeshWriter.h>
#include <MeshFactory.h>

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
};

TEST_F(PeriodicInflator3DTest, cube) {
    WireNetwork network = load_wire("cube.wire");
    network.compute_connectivity();
    network.scale(Vector3F::Ones() * 2.5); // 5mm cell

    const size_t num_vertices = network.get_num_vertices();
    const size_t num_edges = network.get_num_edges();

    PeriodicInflator3D inflator(network);
    inflator.set_thickness_type(InflatorEngine::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.5);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI face_sources = inflator.get_face_sources();

    save_mesh("inflated_cube.msh", vertices, faces, face_sources.cast<Float>());
}
