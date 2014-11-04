#pragma once

#include <Wires/Inflator/SimpleInflator.h>
#include <Wires/Inflator/WireProfile.h>
#include <WireTest.h>
#include <IO/MeshWriter.h>

class SimpleInflatorTest : public WireTest {
    protected:
        void save_mesh(const std::string& file_name,
                MatrixFr& vertices, MatrixIr& faces) {
            VectorF flat_vertices(vertices.rows() * vertices.cols());
            std::copy(vertices.data(), vertices.data() + flat_vertices.size(), flat_vertices.data());
            VectorI flat_faces(faces.rows() * faces.cols());
            std::copy(faces.data(), faces.data() + flat_faces.size(), flat_faces.data());
            VectorI flat_voxels = VectorI::Zero(0);
            MeshWriter::create_writer(file_name)->write(
                    flat_vertices, flat_faces, flat_voxels,
                    vertices.cols(), faces.cols(), 4);
        }
};

TEST_F(SimpleInflatorTest, 2D) {
    WireNetwork network = load_wire("square.wire");
    network.compute_connectivity();
    const size_t num_edges = network.get_num_edges();

    SimpleInflator inflator(network);
    inflator.set_thickness_type(SimpleInflator::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.2);
    inflator.inflate();
    //inflator.refine("loop", 2);

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(faces.rows(), face_sources.size());

    save_mesh("tmp.obj", vertices, faces);
}

TEST_F(SimpleInflatorTest, 3D) {
    WireNetwork network = load_wire("cube.wire");
    network.compute_connectivity();
    const size_t num_edges = network.get_num_edges();

    WireProfile::Ptr profile = WireProfile::create("hexagon");

    SimpleInflator inflator(network);
    inflator.set_profile(profile);
    inflator.set_thickness_type(SimpleInflator::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.25);
    inflator.inflate();
    //inflator.refine("loop", 2);

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(3, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(faces.rows(), face_sources.size());

    //save_mesh("tmp.obj", vertices, faces);
}
